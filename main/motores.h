/**
 * @file motores.h
 * @brief C-compatible motor controller implementation (struct + functions).
 * @details
 * Replaces the previous C++ class with a plain C interface so C files
 * can include and use the motor controller.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "driver/ledc.h"

/* Direction helpers */
#define ABRIR false
#define CERRAR true

struct motores {
    gpio_num_t clk;
    gpio_num_t dt;
    gpio_num_t ena;
    gpio_num_t ph;
    gpio_num_t sleep;

    uint16_t max_pos;
    uint16_t min_pos;

    bool last_state;
    uint16_t position;

    bool until;
    uint16_t objective;

    ledc_channel_t pwm_channel;
};

/* API */
static inline void motores_setup_rotary(struct motores *m)
{
    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << m->clk) | (1ULL << m->dt);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);
    m->last_state = gpio_get_level(m->dt);
}

static inline void motores_setup_motor(struct motores *m)
{
    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << m->ph) | (1ULL << m->sleep);
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config(&io_conf);

    gpio_set_level(m->sleep, 1);

    ledc_timer_config_t ledc_timer = {0};
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.freq_hz = 5000;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {0};
    ledc_channel.gpio_num = m->ena;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel = m->pwm_channel;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;
    ledc_channel_config(&ledc_channel);
}

static inline void motores_init(struct motores *m, gpio_num_t clk_pin, gpio_num_t dt_pin, gpio_num_t en_pin, gpio_num_t ph_pin, gpio_num_t sleep_pin, uint16_t max_pos, uint16_t min_pos, ledc_channel_t pwm_channel)
{
    m->clk = clk_pin;
    m->dt = dt_pin;
    m->ena = en_pin;
    m->ph = ph_pin;
    m->sleep = sleep_pin;
    m->max_pos = max_pos;
    m->min_pos = min_pos;
    m->position = 0;
    m->pwm_channel = pwm_channel;
    m->until = false;
    m->objective = 0;
    m->last_state = false;
    motores_setup_rotary(m);
    motores_setup_motor(m);
}

static inline void motores_step(struct motores *m)
{
    bool A = gpio_get_level(m->dt);
    bool B = gpio_get_level(m->clk);

    if (A != m->last_state)
    {
        if (B == A)
        {
            if (m->position > m->min_pos)
                m->position--;
            else
                ; // reached min, stop handled elsewhere
        }
        else
        {
            if (m->position < m->max_pos)
                m->position++;
            else
                ; // reached max
        }
    }

    if (m->until && (m->position == m->objective))
    {
        m->until = false;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, 0);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
    }

    m->last_state = A;
}

static inline uint16_t motores_read_position(struct motores *m)
{
    return m->position;
}

static inline void motores_set_position(struct motores *m, uint16_t position)
{
    m->position = position;
}

static inline bool motores_start_rotation(struct motores *m, bool direction, uint16_t velocity)
{
    bool arrivedToLimit = false;

    if (direction == ABRIR)
    {
        if (m->position > m->min_pos)
        {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, velocity);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
            gpio_set_level(m->ph, direction);
            arrivedToLimit = false;
        }
        else
        {
            arrivedToLimit = true;
            ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
        }
    }
    else
    {
        if (m->position < m->max_pos)
        {
            ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, velocity);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
            gpio_set_level(m->ph, direction);
            arrivedToLimit = false;
        }
        else
        {
            arrivedToLimit = true;
            ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
        }
    }
    return arrivedToLimit;
}

static inline bool motores_start_until(struct motores *m, bool direction, uint16_t objective, uint16_t velocity)
{
    bool arrivedToObjective = false;
    bool arrivedToLimit = false;
    m->objective = objective;
    m->until = true;

    if (direction == ABRIR)
    {
        if (m->position > m->objective)
        {
            arrivedToLimit = motores_start_rotation(m, direction, velocity);
            arrivedToObjective = false;
        }
        else
        {
            arrivedToObjective = true;
            m->until = false;
            ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
        }
    }
    else
    {
        if (m->position < m->objective)
        {
            arrivedToLimit = motores_start_rotation(m, direction, velocity);
            arrivedToObjective = false;
        }
        else
        {
            arrivedToObjective = true;
            m->until = false;
            ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, 0);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
        }
    }
    return (arrivedToObjective || arrivedToLimit);
}

static inline void motores_stop_rotation(struct motores *m)
{
    m->until = false;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, m->pwm_channel);
}
 