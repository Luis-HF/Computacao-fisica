enum EstadoCofre { CONFIGURANDO, FECHADO };
EstadoCofre estadoAtual = CONFIGURANDO;

char senhaMestra[5];
char tentativaAtual[5];
int digitosInseridos = 0;
unsigned long ultimoDebounce = 0;
const int tempoDebounce = 20; 

const char teclas[4][3] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

const byte numParaSeg[] = {
  0b11000000, // 0
  0b11111001, // 1
  0b10100100, // 2
  0b10110000, // 3
  0b10011001, // 4
  0b10010010, // 5
  0b10000010, // 6
  0b11111000, // 7
  0b10000000, // 8
  0b10010000  // 9
};

void setup() {
  DDRD |= (1 << DDD3);
  PORTD |= (1 << PORTD3); 

  DDRD |= 0xF0;   
  PORTD |= 0x07;  

  DDRC |= 0x3F;   
  DDRB |= 0x01;   
  
  PORTC |= 0x3F;
  PORTB |= 0x01;

  DDRB |= 0x3E;
}

void loop() {
  varrerTeclado();
  atualizarDisplay();
}

void varrerTeclado() {
  if ((millis() - ultimoDebounce) < tempoDebounce) return;

  for (int r = 0; r < 4; r++) {
    PORTD = (PORTD | 0xF0) & ~(1 << (r + 4));
    
    for (int c = 0; c < 3; c++) {
      if (!(PIND & (1 << c))) { 
        processarTecla(teclas[r][c]);
        ultimoDebounce = millis();
        while(!(PIND & (1 << c)));
      }
    }
  }
}

void processarTecla(char tecla) {
  if (tecla >= '0' && tecla <= '9') {
    for (int i = 0; i < 4; i++) {
      if (estadoAtual == CONFIGURANDO) {
        senhaMestra[i] = senhaMestra[i + 1];
      } else {
        tentativaAtual[i] = tentativaAtual[i + 1];
      }
    }

    if (estadoAtual == CONFIGURANDO) {
      senhaMestra[4] = tecla;
    } else {
      tentativaAtual[4] = tecla;
    }
    
    if (digitosInseridos < 5) digitosInseridos++;

    atualizarDisplay(); 

    if (digitosInseridos == 5) {
      delay(100);
      if (estadoAtual == CONFIGURANDO) {
        trancarCofre(); 
      } else {
        verificarSenha(); 
      }
      digitosInseridos = 0;
      for(int i=0; i<5; i++) { senhaMestra[i] = 0; tentativaAtual[i] = 0; }
      PORTB &= ~0x3E; 
    }
  }
}

void trancarCofre() {
  estadoAtual = FECHADO;
  PORTD &= ~(1 << PORTD3);
}

void verificarSenha() {
  bool correta = true;
  for (int i = 0; i < 5; i++) {
    if (tentativaAtual[i] != senhaMestra[i]) correta = false;
  }

  if (correta) {
    estadoAtual = CONFIGURANDO;
    PORTD |= (1 << PORTD3);
  }
}

void atualizarDisplay() {
  if (digitosInseridos == 0) {
    PORTB &= ~0x3E;
    return;
  }

  for (int i = 0; i < digitosInseridos; i++) {
    PORTB &= ~0x3E; 

    int indiceArray = 5 - digitosInseridos + i;
    char digitoChar = (estadoAtual == CONFIGURANDO) ? senhaMestra[indiceArray] : tentativaAtual[indiceArray];
    
    if (digitoChar == 0) continue;

    byte segmentos = numParaSeg[digitoChar - '0'];

    PORTC = (PORTC & 0xC0) | (segmentos & 0x3F);
    if (segmentos & 0x40) PORTB |= 0x01; else PORTB &= ~0x01;

    PORTB |= (1 << (i + 1));
    
    delay(2); 
  }
}