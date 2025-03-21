#  Projeto Rede de Balanço Automatizada com Raspberry Pi Pico



Controlador de rede de balanço em miniatura com **controle por botões**, **display OLED** e **duas velocidades**. Desenvolvido para Raspberry Pi Pico usando SDK em C.

##  Funcionalidades Principais
- ✅ **Controle por Botões:**  
  - `Botão A`: Liga/desliga e altera velocidade (Normal ↔ Rápida).  
  - `Botão B`: Desliga completamente o sistema.
- 📟 **Feedback em Tempo Real:**  
  - Display OLED exibe status do motor e velocidade.
- ⚙️ **Controle de Velocidade:**  
  - Modo Normal (1s por passo) e Rápido (0.5s por passo).
- 🔄 **Movimento Realista:**  
  - Ângulo de 30° a 150° para simular o balanço natural.

##  Componentes Utilizados
| Componente              | Especificações                          |
|-------------------------|-----------------------------------------|
| Raspberry Pi Pico       | Microcontrolador RP2040                 |
| Servo Motor (SG90)      | Controle PWM via GPIO3                  |
| Display OLED SSD1306    | 128x64 pixels, comunicação I2C         |
| Botões                  | Resistores pull-up internos (GPIO5/6)  |
| Fios e Protoboard       | Conexões padrão                         |

##  Esquema de Conexões
| Componente       | Pino no Pico | Descrição               |
|------------------|--------------|-------------------------|
| Servo Motor      | GPIO3        | Sinal PWM               |
| Botão A          | GPIO5        | Entrada com pull-up     |
| Botão B          | GPIO6        | Entrada com pull-up     |
| OLED SDA         | GPIO14       | Dado I2C                |
| OLED SCL         | GPIO15       | Clock I2C               |

##  Instalação e Uso
1. **Clone o Repositório:**
   ```bash
   git clone https://github.com/felipecastro-n/prototipo-rede-balanco.git