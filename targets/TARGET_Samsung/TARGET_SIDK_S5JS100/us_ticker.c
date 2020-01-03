/****************************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#if DEVICE_USTICKER

#include <stddef.h>
#include "cmsis.h"
#include "us_ticker_api.h"
#include "us_ticker.h"
#include "PeripheralNames.h"

#define TIMER_TARGET_COUNT_DEFAULT    0xFFFFFFFF

volatile uint32_t us_ticker_initialized = 0;
volatile uint32_t us_user_intset;
volatile uint32_t g_us_last_return = 0;

void us_ticker_enable_interrupt(void);
void us_ticker_disable_interrupt(void);

const ticker_info_t *us_ticker_get_info()
{
    static const ticker_info_t info = {
        1000000,        //1Mhz
        32              //32bit counter
    };
    return &info;
}

static void enable_timer0(void)
{
    putreg32(1, S5JS100_TIMER0_BASE + S5JS100_TIMER_UP_DOWN_SEL); // Set Up count
    putreg32(0x0, S5JS100_TIMER0_BASE + S5JS100_TIMER_LOAD_VALUE);
    putreg32(0x1, S5JS100_TIMER0_BASE + S5JS100_TIMER_CDC_ENABLE);
    putreg32(0, S5JS100_TIMER0_BASE + S5JS100_TIMER_CDC_COUNT_VALUE);
    putreg32(TIMER_TARGET_COUNT_DEFAULT, S5JS100_TIMER0_BASE + S5JS100_TIMER_INT_SEL);
    putreg32(0, S5JS100_TIMER0_BASE + S5JS100_TIMER_INT_ENABLE);
    putreg32(3, S5JS100_TIMER0_BASE + S5JS100_TIMER_CONTROL);
}

static void disable_timer0(void)
{
    putreg32(0, S5JS100_TIMER0_BASE + S5JS100_TIMER_CONTROL);
    putreg32(0, S5JS100_TIMER0_BASE + S5JS100_TIMER_UP_DOWN_SEL);
    putreg32(TIMER_TARGET_COUNT_DEFAULT, S5JS100_TIMER0_BASE + S5JS100_TIMER_LOAD_VALUE);
    putreg32(TIMER_TARGET_COUNT_DEFAULT, S5JS100_TIMER0_BASE + S5JS100_TIMER_INT_SEL);
    putreg32(0, S5JS100_TIMER0_BASE + S5JS100_TIMER_INT_ENABLE);
    putreg32(0x0, S5JS100_TIMER0_BASE + S5JS100_TIMER_CDC_ENABLE);
    putreg32(TIMER_TARGET_COUNT_DEFAULT, S5JS100_TIMER0_BASE + S5JS100_TIMER_CDC_COUNT_VALUE);
}

static void enable_timer1(void)
{
    putreg32(1, S5JS100_TIMER1_BASE + S5JS100_TIMER_UP_DOWN_SEL); // Set Up count
    putreg32(0x0, S5JS100_TIMER1_BASE + S5JS100_TIMER_LOAD_VALUE);
    putreg32(TIMER_TARGET_COUNT_DEFAULT, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_SEL);
    putreg32(0, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_ENABLE);
    putreg32(3, S5JS100_TIMER1_BASE + S5JS100_TIMER_CONTROL);
}

static void disable_timer1(void)
{
    putreg32(0, S5JS100_TIMER1_BASE + S5JS100_TIMER_UP_DOWN_SEL);
    putreg32(0xFFFFFFFF, S5JS100_TIMER1_BASE + S5JS100_TIMER_LOAD_VALUE);
    putreg32(TIMER_TARGET_COUNT_DEFAULT, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_SEL);
    putreg32(0, S5JS100_TIMER1_BASE + S5JS100_TIMER_CONTROL);
    putreg32(0, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_ENABLE);
}

void __us_ticker_irq_handler(void)
{
    us_ticker_disable_interrupt();
    us_ticker_clear_interrupt();
    us_ticker_irq_handler();
}

void us_ticker_init(void)
{
    /**
     ** We use two timer, timer0 channel to be used timer and timer1 to be used compare timer
     **/

    if (!us_ticker_initialized) {
        us_ticker_initialized = 1;
        /* Enable timer0 to timer */
        enable_timer0();
        /* Enable timer1 to compare and Disable timer1 interrupt */
        enable_timer1();
        /* Install the interrupt handler(timer0) */
        NVIC_SetVector((IRQn_Type)S5JS100_IRQ_TINT1, (uint32_t)__us_ticker_irq_handler);
        NVIC_EnableIRQ((IRQn_Type)S5JS100_IRQ_TINT1);
        return;
    }
    /* Re_init should Disable timer1(compare) interrupt */
    us_ticker_disable_interrupt();
    us_ticker_clear_interrupt();
}

uint32_t us_ticker_read()
{
    /* from timer0 read count value */
    if (!us_ticker_initialized) {
        us_ticker_init();
    }
    volatile uint32_t current_count = getreg32(S5JS100_TIMER0_BASE + S5JS100_TIMER_CDC_COUNT_VALUE) / 26;
    g_us_last_return = current_count;
    return current_count;
}

void us_ticker_set_interrupt(timestamp_t timestamp)
{
    if (timestamp < 0x70000000 && timestamp != 0) {
        uint32_t temp = g_us_last_return;
        us_user_intset = timestamp - g_us_last_return;
        /* keep to check
        if (us_user_intset < 0) {
            us_ticker_irq_handler();
            return;
        }*/
        us_user_intset = us_user_intset * 26;
        uint32_t past_tick = us_ticker_read() - temp;
        putreg32(0, S5JS100_TIMER1_BASE + S5JS100_TIMER_LOAD_CON_VALUE);
        putreg32(us_user_intset - past_tick * 26, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_SEL);
        us_ticker_enable_interrupt();
    } else {
        putreg32(0, S5JS100_TIMER1_BASE + S5JS100_TIMER_LOAD_CON_VALUE);
        putreg32(TIMER_TARGET_COUNT_DEFAULT, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_SEL);
        us_ticker_enable_interrupt();
    }
}

void us_ticker_fire_interrupt(void)
{
    NVIC_SetPendingIRQ((IRQn_Type)S5JS100_IRQ_TINT1);
}

void us_ticker_disable_interrupt(void)
{
    putreg32(0, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_ENABLE);
}

void us_ticker_enable_interrupt(void)
{
    putreg32(1, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_ENABLE);
}

void us_ticker_clear_interrupt(void)
{
    putreg32(1, S5JS100_TIMER1_BASE + S5JS100_TIMER_INT_CLEAR);
    NVIC_ClearPendingIRQ((IRQn_Type)S5JS100_IRQ_TINT1);
}

void us_ticker_free(void)
{
    us_ticker_initialized = 0;
    disable_timer1();
    NVIC_DisableIRQ((IRQn_Type)S5JS100_IRQ_TINT1);
    disable_timer0();
}

#endif // DEVICE_USTICKER
