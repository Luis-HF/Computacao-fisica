#ifndef _DEF_PRINCIPAIS_H
#define _DEF_PRINCIPAIS_H

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define set_bit(y,bit)  (y|=(1<<bit))
#define clr_bit(y,bit)  (y&=~(1<<bit))
#define cpl_bit(y,bit)  (y^=(1<<bit))
#define tst_bit(y,bit)  (y&(1<<bit))
#endif

#ifndef _LCD_H
#define _LCD_H

#define DADOS_LCD PORTD
#define nibble_dados  1
#define CONTR_LCD PORTB
#define E PB1
#define RS  PB0

#define tam_vetor 5
#define conv_ascii  48

#define pulso_enable()  _delay_us(1); set_bit(CONTR_LCD,E); _delay_us(1); clr_bit(CONTR_LCD,E); _delay_us(45)

void cmd_LCD(unsigned char c, char cd);
void inic_LCD_4bits();    
void escreve_LCD(char *c);
void escreve_LCD_Flash(const char *c);
void ident_num(unsigned int valor, unsigned char *disp);
#endif

// Definição dos pinos do aquecedor, motor e buzzer para ligar e desligar
#define MOTOR_PIN PD2
#define HEAT_PIN  PD3 // Aquecedor alocado estritamente no pino 3 (PD3)
#define BUZZ_PIN  PC1

#define MOTORON  PORTD |= (1 << MOTOR_PIN)
#define MOTOROFF PORTD &= ~(1 << MOTOR_PIN)
#define HEATON   PORTD |= (1 << HEAT_PIN)
#define HEATOFF  PORTD &= ~(1 << HEAT_PIN)
#define BUZZCOM  PORTC ^= (1 << BUZZ_PIN)
#define BUZZOFF  PORTC &= ~(1 << BUZZ_PIN)

volatile unsigned long my_millis = 0;
volatile unsigned int beep_period = 0;

//Debaunce razoável--
typedef struct {
    uint8_t pin;
    uint8_t state;
    uint8_t last_reading;
    unsigned long last_debounce_time;
    bool pressed;
} Button;

void update_button(Button *b) {
    uint8_t reading = (PINC & (1 << b->pin)) ? 1 : 0;
    
    if (reading != b->last_reading) {
        b->last_debounce_time = my_millis;
    }
    
    //50ms
    if ((my_millis - b->last_debounce_time) > 50) {
        if (reading != b->state) {
            b->state = reading;
            if (b->state == 0) { // Lógica Pull-up (0 = acionado)
                b->pressed = true;
            }
        }
    }
    b->last_reading = reading;
}

// Instâncias dos botões: PC2 (-), PC3 (+), PC4 (Next)
Button btn_minus = {PC2, 1, 1, 0, false};
Button btn_plus  = {PC3, 1, 1, 0, false};
Button btn_next  = {PC4, 1, 1, 0, false};

unsigned char estado = 0;
// Tempos padrões em milissegundos: 25 min, 90 min, 40 min
unsigned long tempo_fase[3] = {25UL * 60 * 1000, 90UL * 60 * 1000, 40UL * 60 * 1000};
unsigned long tempo_troca = 0;
unsigned long ultimo_update_lcd = 0;
unsigned long tempo_beep_fim = 0;

void print_time(unsigned long ms) {
    uint8_t h = (ms / 3600000UL) % 24;
    uint8_t m = (ms / 60000UL) % 60;
    uint8_t s = (ms / 1000UL) % 60;
    cmd_LCD(h/10 + '0', 1);
    cmd_LCD(h%10 + '0', 1);
    cmd_LCD(':', 1);
    cmd_LCD(m/10 + '0', 1);
    cmd_LCD(m%10 + '0', 1);
    cmd_LCD(':', 1);
    cmd_LCD(s/10 + '0', 1);
    cmd_LCD(s%10 + '0', 1);
}

void exibe_tela_config() {
    cmd_LCD(0x80, 0); // Linha 1
    if (estado == 0)      escreve_LCD((char*)"Cfg: Sova       ");
    else if (estado == 1) escreve_LCD((char*)"Cfg: Descanso   ");
    else if (estado == 2) escreve_LCD((char*)"Cfg: Assadura   ");
    
    cmd_LCD(0xC0, 0); // Linha 2
    print_time(tempo_fase[estado]);
    escreve_LCD((char*)" (+/-)  ");
}

void exibe_tela_execucao(unsigned long faltante) {
    cmd_LCD(0x80, 0);
    if (estado == 3)      escreve_LCD((char*)"Exe: Sova       ");
    else if (estado == 4) escreve_LCD((char*)"Exe: Descanso   ");
    else if (estado == 5) escreve_LCD((char*)"Exe: Assadura   ");

    cmd_LCD(0xC0, 0);
    print_time(faltante);
    
    if (estado == 5) escreve_LCD((char*)" T:150C ");
    else             escreve_LCD((char*)" T:25C  ");
}

int main() {
    cli(); // Desabilita interrupções globais durante o setup

    // I/O Direção e Estado Inicial
    DDRD |= (1 << MOTOR_PIN) | (1 << HEAT_PIN);
    MOTOROFF;
    HEATOFF;

    DDRC |= (1 << BUZZ_PIN);
    BUZZOFF;
    
    DDRC &= ~((1 << PC2) | (1 << PC3) | (1 << PC4)); // Configura como entrada
    PORTC |= (1 << PC2) | (1 << PC3) | (1 << PC4);   // Ativa resistores Pull-up

    DDRB |= (1 << PB0) | (1 << PB1); // Pinos de controle LCD

    // Configuração de Hardware do Timer1 para base de tempo 1ms (Modo CTC)
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11); // Modo CTC, Prescaler 8
    OCR1A = 1999;                        // (16MHz / 8 / 1000Hz) - 1
    TIMSK1 = (1 << OCIE1A);              // Habilita interrupção por comparação

    sei(); // Habilita interrupções globais

    inic_LCD_4bits();
    beep_period = 200; // Aciona o bip curto inicial exigido

    while (1) {
        // Atualiza leitura de botões continuamente
        update_button(&btn_minus);
        update_button(&btn_plus);
        update_button(&btn_next);

        // ESTADOS DE CONFIGURAÇÃO (0, 1 e 2)
        if (estado <= 2) {
            if (btn_minus.pressed) {
                btn_minus.pressed = false;
                if (tempo_fase[estado] >= 300000UL) tempo_fase[estado] -= 300000UL;
                else tempo_fase[estado] = 0;
                beep_period = 50;
            }
            if (btn_plus.pressed) {
                btn_plus.pressed = false;
                tempo_fase[estado] += 300000UL;
                beep_period = 50;
            }
            if (btn_next.pressed) {
                btn_next.pressed = false;
                estado++;
                beep_period = 100;
                
                // Transição para execução da primeira fase (Sova)
                if (estado == 3) {
                    tempo_troca = my_millis + tempo_fase[0];
                    MOTORON;
                    HEATOFF;
                }
            }
            
            // Taxa de atualização do display (throttle) de 200ms
            if (my_millis - ultimo_update_lcd > 200) {
                exibe_tela_config();
                ultimo_update_lcd = my_millis;
            }
        } 
        // ESTADOS DE EXECUÇÃO (3, 4 e 5)
        else if (estado >= 3 && estado <= 5) {
            unsigned long tempo_faltante = 0;
            
            if (tempo_troca > my_millis) {
                tempo_faltante = tempo_troca - my_millis;
            } else {
                // Fim do tempo da fase atual, executa transição
                estado++;
                beep_period = 200;
                
                if (estado == 4) { // Início Descanso
                    MOTOROFF;
                    HEATOFF;
                    tempo_troca = my_millis + tempo_fase[1];
                } else if (estado == 5) { // Início Assadura
                    MOTOROFF;
                    HEATON;
                    tempo_troca = my_millis + tempo_fase[2];
                } else if (estado == 6) { // Fim do processo
                    MOTOROFF;
                    HEATOFF;
                }
            }
            
            if (my_millis - ultimo_update_lcd > 200) {
                exibe_tela_execucao(tempo_faltante);
                ultimo_update_lcd = my_millis;
            }
        } 
        // ESTADO DE CONCLUSÃO (6)
        else if (estado == 6) {
            if (my_millis - ultimo_update_lcd > 200) {
                cmd_LCD(0x80, 0);
                escreve_LCD((char*)"      Fim!      ");
                cmd_LCD(0xC0, 0);
                escreve_LCD((char*)" Retire a Massa ");
                ultimo_update_lcd = my_millis;
            }
            
            // Bip rápido intermitente de alerta
            if (my_millis - tempo_beep_fim > 500) {
                beep_period = 100;
                tempo_beep_fim = my_millis;
            }

            // Permite reiniciar a máquina
            if (btn_next.pressed) {
                btn_next.pressed = false;
                estado = 0;
                BUZZOFF;
            }
        }
    }
    return 0;
}

// Rotina de Serviço de Interrupção do Timer1 (Disparada a cada 1ms)
ISR(TIMER1_COMPA_vect) {
    my_millis++;
    
    // Tratamento de hardware não-bloqueante para o buzzer
    if (beep_period > 0) {
        beep_period--;
        BUZZCOM; 
    } else {
        BUZZOFF;
    }
}