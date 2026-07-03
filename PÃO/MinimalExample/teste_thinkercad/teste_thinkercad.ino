#ifndef _DEF_PRINCIPAIS_H
#define _DEF_PRINCIPAIS_H

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#define	set_bit(y,bit)	(y|=(1<<bit))
#define	clr_bit(y,bit)	(y&=~(1<<bit))
#define cpl_bit(y,bit) 	(y^=(1<<bit))
#define tst_bit(y,bit) 	(y&(1<<bit))

#endif

#ifndef _LCD_H
#define _LCD_H

#define DADOS_LCD PORTD
#define nibble_dados  1
#define CONTR_LCD PORTB
#define E PB1
#define RS  PB0

#define tam_vetor	5
#define conv_ascii  48

#define pulso_enable() 	_delay_us(1); set_bit(CONTR_LCD,E); _delay_us(1); clr_bit(CONTR_LCD,E); _delay_us(45)

void cmd_LCD(unsigned char c, char cd);
void inic_LCD_4bits();		
void escreve_LCD(char *c);
void escreve_LCD_Flash(const char *c);
void ident_num(unsigned int valor, unsigned char *disp);

#endif

unsigned long cliquePC2 = 0;
unsigned long cliquePC3 = 0;
unsigned long cliquePC4 = 0;
unsigned long cliquePC5 = 0;

char last_statePC2 = (1 << PC2);
char last_statePC3 = (1 << PC3);
char last_statePC4 = (1 << PC4);
char last_statePC5 = (1 << PC5);

volatile unsigned long my_millis = 0;
unsigned long tempo_troca;
unsigned long tempo_ultima_checagem = 0;
unsigned long tempo_ultimo_refresh_lcd = 0;

#define MOTORON PORTD |= 1 << PD2
#define MOTOROFF PORTD &= ~(1 << PD2)

#define HEATON PORTD |= 1 << PD3
#define HEATOFF PORTD &= ~(1 << PD3)

#define BUZZON PORTC |= 1 << PC1
#define BUZZOFF PORTC &= ~(1 << PC1)
#define BUZZCOM PORTC ^= (1 << PC1)

volatile unsigned int beep_period;
#define BEEP() beep_period = 200
unsigned char estado = 0;

void exibe_tempo_lcd(unsigned long milissegundos)
{
  unsigned char horas = milissegundos / 3600000UL;
  unsigned char minutos = (milissegundos / 60000UL) % 60;
  unsigned char segundos = (milissegundos / 1000UL) % 60;

  cmd_LCD(horas + '0', 1);
  cmd_LCD(':', 1);
  cmd_LCD((minutos / 10) + '0', 1);
  cmd_LCD((minutos % 10) + '0', 1);
  cmd_LCD(':', 1);
  cmd_LCD((segundos / 10) + '0', 1);
  cmd_LCD((segundos % 10) + '0', 1);
}

int main()
{
  cli();

  MOTOROFF;
  HEATOFF;
  BUZZON;

  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11); 
  TCCR1C = 0;
  OCR1A = 1999;                        
  TIMSK1 = 1 << OCIE1A;

  DDRD = 0xFF;
  DDRB = 1 << PB0 | 1 << PB1;
  DDRC = 1 << PC1;

  DDRC &= ~(1 << PC2 | 1 << PC3 | 1 << PC4 | 1 << PC5);
  PORTC |= (1 << PC2 | 1 << PC3 | 1 << PC4 | 1 << PC5);

  sei();

  inic_LCD_4bits();

  BEEP();

  unsigned long tempo[3];
  tempo[0] = 25UL * 60UL * 1000UL; 
  tempo[1] = 90UL * 60UL * 1000UL; 
  tempo[2] = 40UL * 60UL * 1000UL; 
  
  unsigned long tempo_faltante;

  while (1)
  {
    if (my_millis - tempo_ultimo_refresh_lcd > 200)
    {
      tempo_ultimo_refresh_lcd = my_millis;

      if (estado == 0)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD((char*)"Sova     ");
        cmd_LCD(0x89, 0);
        exibe_tempo_lcd(tempo[estado]);
      }
      else if (estado == 1)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD((char*)"Descanso ");
        cmd_LCD(0x89, 0);
        exibe_tempo_lcd(tempo[estado]);
      }
      else if (estado == 2)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD((char*)"Assadura ");
        cmd_LCD(0x89, 0);
        exibe_tempo_lcd(tempo[estado]);
      }
      else if ((estado >= 3) && (estado <= 5))
      {
        if (tempo_troca > my_millis) {
          tempo_faltante = (tempo_troca - my_millis);
        } else {
          tempo_faltante = 0;
        }
        cmd_LCD(0x89, 0);
        exibe_tempo_lcd(tempo_faltante);
      }
      else if (estado == 6)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD((char*)"      Fim!      ");
        cmd_LCD(0xC0, 0);
        escreve_LCD((char*)"    Belo Pao    ");
      }
    }

    if (estado <= 2)
    {
      char leituraPC2 = PINC & (1 << PC2);
      if (leituraPC2 == 0 && last_statePC2 != 0 && (my_millis - cliquePC2) > 250)
      {
        cliquePC2 = my_millis;
        BEEP();
        if (tempo[estado] > 300000UL) {
          tempo[estado] -= 300000UL; 
        } else if (tempo[estado] == 300000UL) {
          tempo[estado] = 10000UL; 
        } else {
          tempo[estado] = 10000UL; 
        }
      }
      last_statePC2 = leituraPC2;

      char leituraPC3 = PINC & (1 << PC3);
      if (leituraPC3 == 0 && last_statePC3 != 0 && (my_millis - cliquePC3) > 250)
      {
        cliquePC3 = my_millis;
        BEEP();
        if (tempo[estado] == 10000UL) {
          tempo[estado] = 300000UL; 
        } else {
          tempo[estado] += 300000UL; 
        }
      }
      last_statePC3 = leituraPC3;

      char leituraPC4 = PINC & (1 << PC4);
      if (leituraPC4 == 0 && last_statePC4 != 0 && (my_millis - cliquePC4) > 250)
      {
        cliquePC4 = my_millis;
        BEEP();
        estado = estado + 1;
        if (estado == 3)
        {
          tempo_troca = my_millis + tempo[0];
          MOTORON;
          cmd_LCD(0x80, 0);
          escreve_LCD((char*)"Sovando  ");
        }
      }
      last_statePC4 = leituraPC4;

      char leituraPC5 = PINC & (1 << PC5);
      if (estado != 0)
      {
        if (leituraPC5 == 0 && last_statePC5 != 0 && (my_millis - cliquePC5) > 250)
        {
          cliquePC5 = my_millis;
          BEEP();
          estado = estado - 1;
        }
      }
      last_statePC5 = leituraPC5;
    }

    if (estado == 3)
    {
      if (my_millis > tempo_troca)
      {
        BEEP();
        estado = estado + 1;
        tempo_troca = my_millis + tempo[1];
        MOTOROFF;
        cmd_LCD(0x80, 0);
        escreve_LCD((char*)"Crescendo");
      }
    }
    
    if (estado == 4)
    {
      if (my_millis > tempo_troca)
      {
        BEEP();
        estado = estado + 1;
        tempo_troca = my_millis + tempo[2];
        HEATON;
        cmd_LCD(0x80, 0);
        escreve_LCD((char*)"Assando  ");
      }
    }
    
    if (estado == 5)
    {
      if (my_millis > tempo_troca)
      {
        BEEP();
        estado = estado + 1;
        HEATOFF;
      }
    }
    
    if (estado == 6)
    {
      char leituraPC4 = PINC & (1 << PC4);
      if (leituraPC4 == 0 && last_statePC4 != 0 && (my_millis - cliquePC4) > 250)
      {
        cliquePC4 = my_millis;
        estado = 0;
        
        // Força sincronização imediata da tela para extinguir a mensagem "Belo Pao"
        tempo_ultimo_refresh_lcd = my_millis - 200;
        tempo_ultima_checagem = my_millis - 1000;
      }
      last_statePC4 = leituraPC4;
    }

    // Rotina de exibição da temperatura na linha inferior
    if (my_millis - tempo_ultima_checagem > 1000)
    {
      tempo_ultima_checagem = my_millis;
      
      // Bloqueia a exibição da temperatura se a máquina estiver no estágio final
      if (estado != 6) 
      {
        cmd_LCD(0xC0, 0);
        escreve_LCD((char*)"Temp: 67");
        cmd_LCD(0xDF, 1); 
        cmd_LCD('C', 1);
        escreve_LCD((char*)"      "); // Preenche os 6 espaços restantes para fechar as 16 colunas
      }
    }
  }
  return 0;
}

ISR(TIMER1_COMPA_vect)
{
  my_millis += 1;
  if (beep_period > 0)
  {
    beep_period--;
    BUZZCOM;
  }
  else
  {
    BUZZOFF;
  }
}