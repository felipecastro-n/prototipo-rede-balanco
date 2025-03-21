#include <stdio.h>
#include <string.h>  
#include <stdlib.h> 
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"

#define SERVO_PIN 3
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define PWM_WRAP 39062
#define MIN_PULSE 977
#define MAX_PULSE 4883
#define SPEED_NORMAL 1000
#define SPEED_FAST 500

// Configuração do OLED (I2C)
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15

// Variáveis globais
volatile bool motor_running = false;
volatile bool first_movement = true;
volatile int motor_speed = SPEED_NORMAL;
uint8_t display_buffer[ssd1306_buffer_length];  // Buffer para o display
struct render_area display_area;  // Área de renderização

// Função para converter ângulo para valor de pulso PWM
uint16_t angle_to_pulse(int angle) {
    // Garante que o ângulo está entre 0 e 180
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    // Mapeia o ângulo (0-180) para o valor de pulso (MIN_PULSE-MAX_PULSE)
    return MIN_PULSE + (angle * (MAX_PULSE - MIN_PULSE) / 180);
}

// Função para inicializar o OLED
void init_oled() {
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display
    display_area.start_column = 0;
    display_area.end_column = ssd1306_width - 1;
    display_area.start_page = 0;
    display_area.end_page = ssd1306_n_pages - 1;

    calculate_render_area_buffer_length(&display_area);

    // zera o display inteiro
    memset(display_buffer, 0, ssd1306_buffer_length);
    render_on_display(display_buffer, &display_area);
    
    // Exibe mensagem inicial
    ssd1306_draw_string(display_buffer, 0, 0, "Sistema Pronto!");
    render_on_display(display_buffer, &display_area);
}

// Função para atualizar o OLED
void update_display(const char *line1, const char *line2) {
    // Limpa o buffer
    memset(display_buffer, 0, ssd1306_buffer_length);
    
    // Desenha as strings
    ssd1306_draw_string(display_buffer, 0, 0, line1);
    ssd1306_draw_string(display_buffer, 0, 16, line2);
    
    // Atualiza o display
    render_on_display(display_buffer, &display_area);
}

// Callback dos botões
void button_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN) {
        if (!motor_running) {
            motor_running = true;
            motor_speed = SPEED_NORMAL;
            update_display("Estado: LIGADO", "Velocidade:NORMAL");
        
        } else {
            motor_speed = (motor_speed == SPEED_NORMAL) ? SPEED_FAST : SPEED_NORMAL;
            const char *velocidade = (motor_speed == SPEED_NORMAL) ? "NORMAL" : "RAPIDA";
            char msg[32];
            snprintf(msg, sizeof(msg), "Velocidade: %s", velocidade);
            update_display("Estado: LIGADO", msg);
        }
    } else if (gpio == BUTTON_B_PIN) {
        motor_running = false;
        first_movement = true;
        update_display("Estado: DESLIGADO", "");
    }
}

int main() {
    stdio_init_all();
    init_oled();  // Inicializa o OLED

    // Configuração do PWM e GPIOs
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_config_set_clkdiv(&config, 64.0f);
    pwm_init(slice_num, &config, true);

    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);

    pwm_set_gpio_level(SERVO_PIN, angle_to_pulse(90));
    int current_angle = 90;

    while(true) {
        if (motor_running) {
            if (first_movement) {
                // Primeiro movimento: 90° -> 30° (60° para direita)
                current_angle = 30;
                pwm_set_gpio_level(SERVO_PIN, angle_to_pulse(current_angle));
                sleep_ms(motor_speed);
                
                // Depois 30° -> 150° (120° para esquerda)
                current_angle = 150;
                pwm_set_gpio_level(SERVO_PIN, angle_to_pulse(current_angle));
                sleep_ms(motor_speed);
                
                first_movement = false;
            } else {
                // Movimento contínuo: 150° -> 30° (120° para direita)
                current_angle = 30;
                pwm_set_gpio_level(SERVO_PIN, angle_to_pulse(current_angle));
                sleep_ms(motor_speed);
                
                // Depois 30° -> 150° (120° para esquerda)
                current_angle = 150;
                pwm_set_gpio_level(SERVO_PIN, angle_to_pulse(current_angle));
                sleep_ms(motor_speed);
            }
        } else {
            // Quando o motor está desligado, mantém na posição 90°
            pwm_set_gpio_level(SERVO_PIN, angle_to_pulse(90));
            sleep_ms(100);  // Pequeno delay para não sobrecarregar o CPU
        }
    }

    return 0;
}