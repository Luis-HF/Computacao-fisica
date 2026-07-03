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


#define MAX_TEMP 212.f
#define MIN_TEMP 11.f
#define MAX_COUNT 1011
#define MIN_COUNT 79

unsigned int adc_res;

float temperature_read = 0.0;

#define TEMPERATURA_ASSAR_MAX 150
#define TEMPERATURA_ASSAR_MIN 60

unsigned long cliquePC2 = 0;
unsigned long cliquePC3 = 0;
unsigned long cliquePC4 = 0;
unsigned long cliquePC5 = 0;
char last_statePC2 = (1 << PC2);
char last_statePC3 = (1 << PC3);
char last_statePC4 = (1 << PC4);
char last_statePC5 = (1 << PC5);

#define num_ADC_average 32
volatile unsigned int adc_result0[num_ADC_average];
volatile unsigned int adc_pos0 = num_ADC_average - 1;

volatile unsigned long my_millis = 0;
volatile unsigned long tempo_troca;
volatile unsigned long tempo_ultima_checagem = 0;
#define MOTORON PORTD |= 1 << PD2
#define MOTOROFF PORTD &= ~(1 << PD2)

#define HEATON PORTD |= 1 << PD3
#define HEATOFF PORTD &= ~(1 << PD3)

#define BUZZON PORTC |= 1 << PC1
#define BUZZOFF PORTC &= ~(1 << PC1)
#define BUZZCOM PORTC ^= (1 << PC1)

#define COUNT2TEMP(c) (MIN_TEMP + (((MAX_TEMP - MIN_TEMP) / (MAX_COUNT - MIN_COUNT)) * (((float)(c)) - MIN_COUNT)))

volatile unsigned int beep_period;

#define BEEP() beep_period = 200
unsigned char estado = 0;
int main()
{
  cli();

  MOTOROFF;
  HEATOFF;
  BUZZON;

  ADMUX = 0;
  ADCSRA = 1 << ADEN | 1 << ADIE;
  ADCSRA |= 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0;
  DIDR0 = 1 << ADC0D;

  TCCR1A = 0;
  TCCR1B = (1 << CS11);
  TCCR1C = 0;
  OCR1A = 0x07CF;
  TIMSK1 = 1 << OCIE1A;

  DDRD = 0xFF;
  DDRB = 1 << PB0 | 1 << PB1;
  DDRC = 1 << PC1;

  DDRC &= ~(1 << PC2 | 1 << PC3 | 1 << PC4 | 1 << PC5);
  PORTC |= (1 << PC2 | 1 << PC3 | 1 << PC4 | 1 << PC5);

  sei();

  Serial.begin(115200);

  ADCSRA |= 1 << ADSC;

  inic_LCD_4bits();

  BEEP();
  volatile unsigned int tempo[3];
  tempo[0] = 25 * 60 * 1000;
  tempo[1] = 90 * 60 * 1000;
  tempo[2] = 40 * 60 * 1000;
  volatile unsigned int tempo_faltante;
  char unidade_hora, dezena_minuto, unidade_minuto, dezena_segundo, unidade_segundo;
  while (1)
  {

    cmd_LCD(0x80, 0);

    if (estado == 0)
    {

      unidade_segundo = ((tempo[estado] / 1000) % 60) % 10;
      dezena_segundo = ((tempo[estado] / 1000) % 60) / 10;
      unidade_minuto = (((tempo[estado] / 1000) / 60) % 60) % 10;
      dezena_minuto = (((tempo[estado] / 1000) / 60) % 60) / 10;
      unidade_hora = (((tempo[estado] / 1000) / 60) / 60) % 10;

      escreve_LCD("Sovar    ");
          cmd_LCD(0x89, 0);
      cmd_LCD(unidade_hora + '0', 1);
      cmd_LCD(':', 1);
      cmd_LCD(dezena_minuto + '0', 1);
      cmd_LCD(unidade_minuto + '0', 1);
      cmd_LCD(':', 1);
      cmd_LCD(dezena_segundo + '0', 1);
      cmd_LCD(unidade_segundo + '0', 1);
    }
    if (estado == 1)
    {

      unidade_segundo = ((tempo[estado] / 1000) % 60) % 10;
      dezena_segundo = ((tempo[estado] / 1000) % 60) / 10;
      unidade_minuto = (((tempo[estado] / 1000) / 60) % 60) % 10;
      dezena_minuto = (((tempo[estado] / 1000) / 60) % 60) / 10;
      unidade_hora = (((tempo[estado] / 1000) / 60) / 60) % 10;

      escreve_LCD("Crescer  ");
          cmd_LCD(0x89, 0);
      cmd_LCD(unidade_hora + '0', 1);
      cmd_LCD(':', 1);
      cmd_LCD(dezena_minuto + '0', 1);
      cmd_LCD(unidade_minuto + '0', 1);
      cmd_LCD(':', 1);
      cmd_LCD(dezena_segundo + '0', 1);
      cmd_LCD(unidade_segundo + '0', 1);
    }
    if (estado == 2)
    {

      unidade_segundo = ((tempo[estado] / 1000) % 60) % 10;
      dezena_segundo = ((tempo[estado] / 1000) % 60) / 10;
      unidade_minuto = (((tempo[estado] / 1000) / 60) % 60) % 10;
      dezena_minuto = (((tempo[estado] / 1000) / 60) % 60) / 10;
      unidade_hora = (((tempo[estado] / 1000) / 60) / 60) % 10;

      escreve_LCD("Assar    ");
          cmd_LCD(0x89, 0);
      cmd_LCD(unidade_hora + '0', 1);
      cmd_LCD(':', 1);
      cmd_LCD(dezena_minuto + '0', 1);
      cmd_LCD(unidade_minuto + '0', 1);
      cmd_LCD(':', 1);
      cmd_LCD(dezena_segundo + '0', 1);
      cmd_LCD(unidade_segundo + '0', 1);
    }
    if ((estado >= 3) && (estado <= 5))
      {
        tempo_faltante = (tempo_troca - my_millis);
        unidade_segundo = ((tempo_faltante / 1000) % 60) % 10;
        dezena_segundo = ((tempo_faltante / 1000) % 60) / 10;
        unidade_minuto = (((tempo_faltante / 1000) / 60) % 60) % 10;
        dezena_minuto = (((tempo_faltante / 1000) / 60) % 60) / 10;
        unidade_hora = (((tempo_faltante / 1000) / 60) / 60) % 10;

        cmd_LCD(0x89, 0);
        cmd_LCD(unidade_hora + '0', 1);
        cmd_LCD(':', 1);
        cmd_LCD(dezena_minuto + '0', 1);
        cmd_LCD(unidade_minuto + '0', 1);
        cmd_LCD(':', 1);
        cmd_LCD(dezena_segundo + '0', 1);
        cmd_LCD(unidade_segundo + '0', 1);
      }
    if (estado == 6)
    {
      cmd_LCD(0x80, 0);
      escreve_LCD("      Fim!      ");
    }

    if (estado <= 2)
    {

      char leituraPC2 = PINC & (1 << PC2);

      if (leituraPC2 != last_statePC2 && (my_millis - cliquePC2) > 1)
      {
        cliquePC2 = my_millis;

        if (leituraPC2 == 0)
        {
          tempo[estado] -= 60000;
        }
      }

      last_statePC2 = leituraPC2;

      char leituraPC3 = PINC & (1 << PC3);

      if (leituraPC3 != last_statePC3 && (my_millis - cliquePC3) > 1)
      {
        cliquePC3 = my_millis;

        if (leituraPC3 == 0)
        {
          tempo[estado] += 60000;
        }
      }

      last_statePC3 = leituraPC3;

      char leituraPC4 = PINC & (1 << PC4);

      if (leituraPC4 != last_statePC4 && (my_millis - cliquePC4) > 1)
      {
        cliquePC4 = my_millis;

        if (leituraPC4 == 0)
        {
          estado = estado + 1;
          BEEP();
          if (estado == 3)
          {
            tempo_troca = my_millis + tempo[estado - 3];
            MOTORON;
            cmd_LCD(0x80, 0);
            escreve_LCD("Sovando");
          }
        }
      }
        last_statePC4 = leituraPC4;

        char leituraPC5 = PINC & (1 << PC5);
        if (estado != 0)
          if (leituraPC5 != last_statePC5 && (my_millis - cliquePC5) > 1)
          {
            cliquePC5 = my_millis;

            if (leituraPC5 == 0)
            {
              estado = estado - 1;
              BEEP();
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
        tempo_troca = my_millis + tempo[estado - 3];
        MOTOROFF;
        cmd_LCD(0x80, 0);
        escreve_LCD("Crescendo");
      }
    }
    if (estado == 4)
    {
      if (my_millis > tempo_troca)
      {
        BEEP();
        estado = estado + 1;
        tempo_troca = my_millis + tempo[estado - 3];
        HEATON;
        cmd_LCD(0x80, 0);
        escreve_LCD("Assando");
      }
    }
    if (estado == 5)
    {
      if (temperature_read > TEMPERATURA_ASSAR_MAX)
        HEATOFF;
      if (temperature_read < TEMPERATURA_ASSAR_MIN)
        HEATON;
      if (my_millis > tempo_troca)
      {
        BEEP();
        estado = estado + 1;
        HEATOFF;
      }
    }
    if (estado == 6)
    {
      BEEP();

      char leituraPC4 = PINC & (1 << PC4);

      if (leituraPC4 != last_statePC4 && (my_millis - cliquePC4) > 1)
      {
        cliquePC4 = my_millis;

        if (leituraPC4 == 0)
        {
          estado = 0;
        }
      }

      last_statePC4 = leituraPC4;
    }
    if (my_millis - tempo_ultima_checagem > 1000)
    {
      tempo_ultima_checagem = my_millis;
      Serial.print(", ADC0: ");
      adc_res = 0;
      for (int i = 0; i < num_ADC_average; i++)
        adc_res += adc_result0[i];
      adc_res /= num_ADC_average;
      Serial.print(adc_res);
      Serial.print(", ");

      temperature_read = COUNT2TEMP(adc_res);
      unsigned char digitos[tam_vetor];
      ident_num(temperature_read, digitos);
      Serial.print(temperature_read);
      Serial.println();
      cmd_LCD(0xC0, 0);
      escreve_LCD("Temperatura");
      cmd_LCD(digitos[2], 1);
      cmd_LCD(digitos[1], 1);
      cmd_LCD(digitos[0], 1);
      cmd_LCD('º', 1);
      cmd_LCD('C', 1);
    }

  }
  return 0;
}

  ISR(ADC_vect)
  {
    adc_result0[adc_pos0++ % num_ADC_average] = ADC;
    ADCSRA |= 1 << ADSC;
  }

  ISR(TIMER1_COMPA_vect)
  {
    OCR1A += 0x07CF;
    my_millis += 1;

    if (beep_period > 0)
    {
      beep_period--;
      BUZZCOM;
    }
  }
