#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

// definição dos digitos
#define DIG1 PD2
#define DIG2 PD3
#define DIG3 PD1
#define DIG4 PD0

// definição dos segmentos
#define SEG_PORTC_MASK 0b00111111 // PC0–PC5
#define SEG_G PD7

// definição do encolder
#define CLK PB0
#define DT  PB1
#define SW  PB2

// buzzer
#define BUZZER PB4
#define CLN PB3

// variaveis utilizadas
volatile uint8_t minutos = 0;
volatile uint8_t segundos = 0;

volatile uint8_t rodando = 0;

volatile uint16_t ms = 0;
volatile uint16_t blink = 0;
volatile uint16_t buzzer_timer = 0;

volatile uint8_t digito = 0;

// tabela 7 segmentos (abcdefg)
const uint8_t tabela[10] = {
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00000111, //7
    0b01111111, //8
    0b01101111  //9
};

// funções

void setSegments(uint8_t val){
    PORTC = (val & 0x3F);

    if(val & (1<<6))
        PORTD |= (1<<SEG_G);
    else
        PORTD &= ~(1<<SEG_G);
}

void desligaDigitos(){
    PORTD &= ~((1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4));
}

void ligaDigito(uint8_t d){
    switch(d){
        case 0: PORTD |= (1<<DIG1); break;
        case 1: PORTD |= (1<<DIG2); break;
        case 2: PORTD |= (1<<DIG3); break;
        case 3: PORTD |= (1<<DIG4); break;
    }
}

void incrementar(){
    segundos++;
    if(segundos == 60){
        segundos = 0;
        minutos++;
        if(minutos == 100) minutos = 0;
    }
}
//função utilizada para incrementar segundos

void decrementar(){
    if(segundos == 0){
        if(minutos > 0){
            minutos--;
            segundos = 59;
        }
    } else {
        segundos--;
    }
}
// contrario da anterior
// tempo
ISR(TIMER0_COMPA_vect){

    ms++;

    // multiplexação
    desligaDigitos();

    uint8_t m1 = minutos / 10;
    uint8_t m2 = minutos % 10;
    uint8_t s1 = segundos / 10;
    uint8_t s2 = segundos % 10;

    switch(digito){
        case 0: setSegments(~tabela[m1]); ligaDigito(0); break;
        case 1: setSegments(~tabela[m2]); ligaDigito(1); break;
        case 2: setSegments(~tabela[s1]); ligaDigito(2); break;
        case 3: setSegments(~tabela[s2]); ligaDigito(3); break;
    }
//liga os digitos de acordo com a tabela, no caso invertida por que é ânodo comun
    digito = (digito + 1) % 4;

    // contagem 
    static uint16_t cont1s = 0;
    cont1s++;

    if(cont1s == 1000){
        cont1s = 0;

        if(rodando){
            decrementar();

            if(minutos == 0 && segundos == 0){
                rodando = 0;
                PORTB |= (1<<BUZZER);
                buzzer_timer = 300;
            }
        }
    }

    // ativação do buzzer
    if(buzzer_timer > 0){
        buzzer_timer--;
        if(buzzer_timer == 0){
            PORTB &= ~(1<<BUZZER);
        }
    }

    //piscar o :
    blink++;

    if(rodando){
        if(blink >= 500){
            PORTB ^= (1<<CLN); // usa G como exemplo de ":" (ajuste se tiver pino separado)
            blink = 0;
        }
    } else {
        PORTB |= (1<<CLN);// sempre ligado (simulado)
    }
}

// definição de tempo inicial 
void timer0_init(){
    TCCR0A = (1<<WGM01);
    TCCR0B = (1<<CS01)|(1<<CS00); // prescaler 64
    OCR0A = 249; // 1ms
    TIMSK0 = (1<<OCIE0A);
}

int main(){

    // PORTC saída (segmentos)
    DDRC = 0x3F;

    // PORTD saída (dígitos + g)
    DDRD = (1<<DIG1)|(1<<DIG2)|(1<<DIG3)|(1<<DIG4)|(1<<SEG_G);

    // PORTB entrada encoder + buzzer saída
    DDRB = (1<<BUZZER) | (1<<CLN);
    PORTB |= (1<<CLK)|(1<<DT)|(1<<SW); // pull-up

    timer0_init();
    sei();

    uint8_t lastCLK = 0;

    while(1){

        uint8_t clk = PINB & (1<<CLK);
//se DT for 1 decrementa, se for 0 incrementa, isso de acordo com a mudança de clk
        if(clk && !lastCLK){
            if(PINB & (1<<DT))
                decrementar();
            else
                incrementar();
        }

        lastCLK = clk;

        // botão
        static uint8_t lastSW = 1;
        uint8_t sw = PINB & (1<<SW);

        if(!sw && lastSW){
            rodando = !rodando;
        }

        lastSW = sw;
    }
}
/*
DIG1 portd 2
DIG2 portd 3
DIG3 portd 1
DIG4 portd 0

segmentos:
A : portc A0
B : portc A1
C : portc A2
d : portc A3
e : portc A4
f : portc A5
g : portd 7

encolder :
CLK : portb 0
DT : portb 1
SW : portb 2
portb 3 temos o CLN do display
+: liga o buzzer 1:2 
buzzer 1:1 fica no portb 4
*/