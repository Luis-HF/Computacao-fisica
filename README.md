# 🛠️ Laboratório de Computação Física & Microcontroladores

Bem-vindo ao repositório central da disciplina de **Computação Física**. Este espaço é dedicado ao estudo e implementação de sistemas embarcados focados no **mais baixo nível**, priorizando a manipulação direta de bits, o entendimento da arquitetura de hardware e a otimização de recursos.

## 🎯 Objetivo do Repositório

O foco aqui não é apenas "fazer funcionar", mas sim entender **como** o hardware opera. Por isso, os projetos evitam abstrações de alto nível (bibliotecas comerciais) em favor de:
- **Manipulação Direta de Registradores:** Uso de `DDR`, `PORT` e `PIN`.
- **Gestão de Interrupções:** Configuração de Timers e interrupções externas.
- **Protocolos de Comunicação:** Implementação bit-bang ou via registradores de UART, I2C e SPI.
- **Eletrônica Fundamental:** Debounce via software, multiplexação e condicionamento de sinais.

---

## 🗂️ Projetos em Destaque

### 1. Cofre Eletrônico (ATMega328P)
Um sistema de segurança completo que utiliza matriz de teclado 4x3 e 5 displays de 7 segmentos.
* **Destaque:** Implementação de *Shift Left* para entrada de dados e multiplexação manual de anodo comum.
* **Tecnologias:** Port Manipulation (Portas B, C, D), lógica booleana para máscaras de bits.
* https://github.com/Luis-HF/Computacao-fisica/tree/master/ATV4

---

## 🏗️ Arquiteturas Exploradas

### **ATMega328P (Arduino Uno)**
Arquitetura AVR de 8 bits. Aqui exploramos o limite do processamento paralelo simulado via multiplexação e o controle estrito de memória.

### **ESP32**
Arquitetura de 32 bits (Dual-Core). Foco em sistemas multitarefa, Wi-Fi/Bluetooth de baixo nível e sensores capacitivos internos.

---

## 💻 Padrão de Codificação (Low-Level)

Ao ler o código deste repositório, você encontrará frequentemente:

```cpp
// Exemplo de configuração de pino sem usar digitalWrite
DDRD |= (1 << PD3);   // Define PD3 como Saída
PORTD &= ~(1 << PD3); // Coloca PD3 em nível LOW
