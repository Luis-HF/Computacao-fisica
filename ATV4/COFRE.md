# Cofre Eletrónico com Arduino - Computação Física

Este projeto consiste num sistema de segurança de um cofre eletrónico desenvolvido no **Tinkercad**, utilizando o microcontrolador **ATmega328P** (Arduino Uno). A implementação foca-se no **baixo nível**, utilizando manipulação direta de registadores para controlo de hardware, sem o uso de bibliotecas externas para o teclado ou display.

## 🚀 Funcionalidades

- **Configuração de Senha:** No estado inicial (aberto), o sistema solicita a gravação de uma senha de 5 dígitos.
- **Segurança Ativa:** Após a configuração, o cofre tranca automaticamente (LED desliga).
- **Deslocamento de Dígitos (Shift Left):** Os números entram pela direita do display e deslocam-se para a esquerda conforme novas teclas são pressionadas.
- **Interface Visual:** Multiplexação de 5 displays de 7 segmentos (Anodo Comum).
- **Feedback de Porta:** LED indicador de estado (Aceso = Aberto / Apagado = Trancado).

---

## 🛠️ Especificações Técnicas (Baixo Nível)

### 1. Manipulação de Registadores
Para otimizar o desempenho e demonstrar o funcionamento interno do processador, o projeto utiliza os registadores de portas:
- **DDRx:** Configuração de direção (Entrada/Saída).
- **PORTx:** Escrita de estados lógicos e ativação de Pull-ups.
- **PINx:** Leitura de estados físicos das colunas do teclado.

### 2. Mapeamento de Hardware

| Componente | Pinos Arduino | Registrador | Notas |
| :--- | :--- | :--- | :--- |
| **Teclado (Linhas)** | PD4, PD5, PD6, PD7 | `PORTD` | Saídas para varredura |
| **Teclado (Colunas)** | PD0, PD1, PD2 | `PIND` | Entradas com Pull-up interno |
| **LED Indicador** | PD3 | `PORTD` | LOW = Fechado / HIGH = Aberto |
| **Displays (Seg. A-F)**| PC0 a PC5 | `PORTC` | Lógica de Anodo Comum (LOW acende) |
| **Display (Seg. G)** | PB0 | `PORTB` | Segmento central |
| **MUX (1-5)** | PB5 a PB1 | `PORTB` | Controlo de ativação do dígito |

### 3. Lógica de Software
- **Debounce:** Implementado via software para evitar múltiplos registos num único clique.
- **Varredura de Matriz:** Algoritmo manual que alterna o estado das linhas para identificar a interseção com a coluna pressionada.
- **Multiplexação:** Varredura rápida (intervalo de 2ms) entre os visores para criar a ilusão de que todos estão ligados simultaneamente através da persistência de visão.

---

## ⚠️ Notas de Eletrónica

O projeto no simulador pode exibir um aviso de **sobrecorrente** (símbolo de explosão) se não forem utilizados resistores de proteção. 
- **Recomendação:** Implementar resistores de **220Ω** em série com os segmentos.
- **Conflito Serial:** Como os pinos `PD0` e `PD1` (RX/TX) são utilizados para o teclado, a comunicação Serial deve ser evitada para não gerar conflitos elétricos no ATmega328P.

---

## 📖 Como Utilizar

1. Copie o código fonte fornecido para o editor do **Tinkercad**.
2. Certifique-se de que os displays de 7 segmentos estão configurados como **Anodo Comum**.
3. Inicie a simulação.
4. Digite 5 números para trancar o cofre.
5. Digite a mesma sequência para o abrir novamente.

---
**Curso:** Bacharelato em Ciência da Computação  
**Disciplina:** Computação Física  
**Semestre:** 4º Período