/**
 * @file motores.h
 * @brief Clase para el control de un motor con encoder rotativo y driver.
 * @details
 * Permite controlar un motor de la prótesis usando un encoder relativo y un driver,
 * con funciones para moverlo a una posición específica o rotar en una dirección determinada.
 * 
 * El sistema incluye:
 * - Límite mínimo y máximo de posición.
 * - Lectura y actualización de la posición con interrupciones.
 * - Control de velocidad mediante PWM.
 * - Control de dirección mediante pin de fase.
 * - Función "until" para parar en una posición objetivo.
 */
#pragma once
#include <cstdint>
#include "driver/gpio.h"
#include "driver/ledc.h"


 /**
  * @brief Dirección de apertura.
  * @details Cuando el motor está completamente abierto, @ref posMotor es igual a min_pos.
  */
#define ABRIR false

 /**
  * @brief Dirección de cierre.
  * @details Cuando el motor está completamente cerrado, @ref posMotor es igual a max_pos.
  */
#define CERRAR true




 /**
  * @class motorController
  * @brief Clase que gestiona un motor con encoder y driver.
  */
class motorController{
    private: 
    gpio_num_t clk;      ///< Pin de señal CLK del encoder.
    gpio_num_t dt;       ///< Pin de señal DT del encoder.
    gpio_num_t ena;      ///< Pin ENABLE PWM del driver (control de velocidad).
    gpio_num_t ph;       ///< Pin PHASE del driver (control de dirección).
    gpio_num_t sleep;    ///< Pin SLEEP del driver (HIGH: activo, LOW: motor libre).

    uint16_t max_pos; ///< Posición máxima permitida.
    uint16_t min_pos; ///< Posición mínima permitida.

    bool last_state;   ///< Último estado leído del encoder.
    uint16_t position; ///< Posición actual del motor.

    bool until;         ///< Flag de modo "until" (parar en objetivo).
    uint16_t objective; ///< Posición objetivo.

    ledc_channel_t pwm_channel; ///< Canal PWM para controlar la velocidad.
    

    void setup_rotary(); ///< Configura pines del encoder.
    void setup_motor();  ///< Configura pines del motor.

 public:
     /**
      * @brief Constructor.
      * @param clk_pin   Pin CLK del encoder (B).
      * @param dt_pin    Pin DT del encoder (A).
      * @param en_pin    Pin ENABLE PWM del driver.
      * @param ph_pin    Pin PHASE del driver.
      * @param sleep_pin Pin SLEEP del driver.
      * @param max_pos   Posición máxima.
      * @param min_pos   Posición mínima.
      */
     motorController(gpio_num_t clk_pin, gpio_num_t dt_pin, gpio_num_t en_pin, gpio_num_t ph_pin, gpio_num_t sleep_pin, uint16_t max_pos, uint16_t min_pos, ledc_channel_t pwm_channel);
 
     /**
      * @brief Actualiza la posición según el encoder.
      * @details Se debe llamar periódicamente o en interrupciones (>200 Hz recomendado).
      */
     void step();
 
     /**
      * @brief Devuelve la posición actual del motor.
      * @return Posición en unidades del encoder.
      */
     uint16_t read_position();
 
     /**
      * @brief Establece manualmente la posición actual del motor.
      * @param position Nueva posición.
      */
     void set_position(uint16_t position);
 
     /**
      * @brief Inicia rotación hasta alcanzar una posición objetivo.
      * @param direction Dirección del movimiento ( @ref ABRIR o @ref CERRAR).
      * @param objective Posición objetivo.
      * @param velocity  Velocidad en valor PWM (0-255).
      * @retval true Si se alcanzó el objetivo o un límite.
      * @retval false Si el motor sigue en movimiento.
      */
     bool start_until(bool direction, uint16_t objective, uint16_t velocity);
 
     /**
      * @brief Inicia la rotación continua en una dirección.
      * @param direction Dirección del movimiento ( @ref ABRIR o @ref CERRAR).
      * @param velocity  Velocidad en valor PWM (0-255).
      * @retval true Si se alcanzó un límite.
      * @retval false Si sigue en movimiento.
      */
     bool start_rotation(bool direction, uint16_t velocity);
 
     /**
      * @brief Detiene la rotación del motor.
      */
     void stop_rotation();
 };

 // ==================== Implementación ====================
 
 motorController::motorController(gpio_num_t clk_pin, gpio_num_t dt_pin, gpio_num_t en_pin, gpio_num_t ph_pin, gpio_num_t sleep_pin, uint16_t max_pos, uint16_t min_pos, ledc_channel_t pwm_channel)
 {
     this->clk = clk_pin;
     this->dt = dt_pin;
     this->ena = en_pin;
     this->ph = ph_pin;
     this->sleep = sleep_pin;
     this->max_pos = max_pos;
     this->min_pos = min_pos;
     this->position = 0;
     this->pwm_channel = pwm_channel;
     this->until = false;
     this->objective = 0;
     this->last_state = false;
     this->setup_rotary();
     this->setup_motor();
 }
 
 void motorController::setup_rotary()
 {
     gpio_config_t io_conf{};

     io_conf.intr_type = GPIO_INTR_DISABLE;
     io_conf.mode = GPIO_MODE_INPUT;
     io_conf.pin_bit_mask = (1ULL << clk) | (1ULL << dt);
     io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
     io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

     gpio_config(&io_conf);
 }
 
 void motorController::setup_motor()
 {
     gpio_config_t io_conf{};

     io_conf.intr_type = GPIO_INTR_DISABLE;
     io_conf.mode = GPIO_MODE_OUTPUT;
     io_conf.pin_bit_mask = (1ULL << ph) | (1ULL << sleep);
     io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
     io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

     gpio_config(&io_conf);

    gpio_set_level(this->sleep, 1);

    
    ledc_timer_config_t ledc_timer{};
    ledc_timer.speed_mode       = LEDC_LOW_SPEED_MODE;;
    ledc_timer.duty_resolution  = LEDC_TIMER_8_BIT;
    ledc_timer.timer_num        = LEDC_TIMER_0;
    ledc_timer.freq_hz          = 5000;        
    ledc_timer.clk_cfg          = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel{};
    ledc_channel.gpio_num   = ena;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel    = pwm_channel;
    ledc_channel.timer_sel  = LEDC_TIMER_0;
    ledc_channel.duty       = 0;
    ledc_channel.hpoint     = 0;
    ledc_channel_config(&ledc_channel);

 }
 
 void motorController::step()
 {
    bool A = gpio_get_level(this->dt);
    bool B = gpio_get_level(this->clk);
 
     if (A != this->last_state)
     {
         if (B == A)
         {
             if (this->position > this->min_pos)
                 this->position--;
             else
                 this->stop_rotation();
         }
         else
         {
             if (this->position < this->max_pos)
                 this->position++;
             else
                 this->stop_rotation();
         }
     }
 
     if (this->until && (this->position == this->objective))
         this->stop_rotation();
 
     this->last_state = A;
     return;
 }
 
 uint16_t motorController::read_position()
 {
     return this->position;
 }
 
 void motorController::set_position(uint16_t position)
 {
     this->position = position;
 }
 
 bool motorController::start_rotation(bool direction, uint16_t velocity)
 {
     bool arrivedToLimit = false;
 
     if (direction == ABRIR)
     {
         if (this->position > this->min_pos)
         {
             ledc_set_duty(LEDC_LOW_SPEED_MODE, this->pwm_channel, velocity);
             ledc_update_duty(LEDC_LOW_SPEED_MODE, this->pwm_channel);

             gpio_set_level(this->ph, direction);
             arrivedToLimit = false;
         }
         else
         {
             arrivedToLimit = true;
             this->stop_rotation();
         }
     }
     else if (direction == CERRAR)
     {
         if (this->position < this->max_pos)
         {
             ledc_set_duty(LEDC_LOW_SPEED_MODE, this->pwm_channel, velocity);
             ledc_update_duty(LEDC_LOW_SPEED_MODE, this->pwm_channel);
             gpio_set_level(this->ph, direction);
             arrivedToLimit = false;
         }
         else
         {
             arrivedToLimit = true;
             this->stop_rotation();
         }
     }
     return arrivedToLimit;
 }
 
 bool motorController::start_until(bool direction, uint16_t objective, uint16_t velocity)
 {
     bool arrivedToObjective = false;
     bool arrivedToLimit = false;
     this->objective = objective;
     this->until = true;
 
     if (direction == ABRIR)
     {
         if (this->position > this->objective) {
             arrivedToLimit = this->start_rotation(direction, velocity);
             arrivedToObjective = false;
         }
         else
         {
             arrivedToObjective = true;
             this->stop_rotation();
         }
     }
     else if (direction == CERRAR)
     {
         if (this->position < this->objective) {
             arrivedToLimit = this->start_rotation(direction, velocity);
             arrivedToObjective = false;
         }
         else
         {
             arrivedToObjective = true;
             this->stop_rotation();
         }
     }
     return (arrivedToObjective || arrivedToLimit);
 }
 
 void motorController::stop_rotation()
 {
     this->until = false;
     ledc_set_duty(LEDC_LOW_SPEED_MODE, this->pwm_channel, 0);
     ledc_update_duty(LEDC_LOW_SPEED_MODE, this->pwm_channel);
 }
 