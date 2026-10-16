/*
 * Copyright (c) 2026-present OwnTech Foundation
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-2.1
 */

/*
 * @date   2026
 *
 * @brief  DMA-driven duty-cycle sequences: DMA2 copies one CMP1 value per
 *         PWM period from a circular buffer into a timing unit, requested
 *         by the timing unit repetition event.
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/dma.h>

#include <stm32_ll_dma.h>
#include <stm32_ll_dmamux.h>

#include "hrtim.h"

/**
 * Period synchronisation busy-waits. The loops only read the timer counter
 * (a few CPU cycles per pass) and are bounded by counting counter wraps:
 * an earlier version called k_cycle_get_32() in each pass, which made the
 * counter sampling ~3 us coarse, missed the [1/2, 3/4) window and detected
 * wraps 1-3 us late. The iteration cap only protects
 * against a stopped timer (a few ms).
 */
#define HRTIM_PERIOD_WAIT_MAX_WRAPS 2U
#define HRTIM_PERIOD_WAIT_MAX_LOOPS 200000U

static inline uint32_t _counter(hrtim_tu_t timer)
{
    return LL_HRTIM_TIM_GetCounter(HRTIM1, timer);
}

void hrtim_wait_period_start(hrtim_tu_number_t tu_number)
{
    hrtim_tu_t timer = tu_channel[tu_number]->pwm_conf.pwm_tu;

    /**
     * The boundary is detected by the counter wrapping, not by the REP flag:
     * once the REP DMA request of the timer is enabled, the DMA acknowledge
     * can clear the flag before it is polled.
     */
    uint32_t previous = _counter(timer);

    for (uint32_t loops = 0; loops < HRTIM_PERIOD_WAIT_MAX_LOOPS; loops++)
    {
        uint32_t now = _counter(timer);
        if (now < previous)
            break;
        previous = now;
    }
}

void hrtim_wait_period_middle(hrtim_tu_number_t tu_number)
{
    hrtim_tu_t timer = tu_channel[tu_number]->pwm_conf.pwm_tu;
    uint32_t period = tu_channel[tu_number]->pwm_conf.period;
    uint32_t low = period / 2;
    uint32_t high = (3 * period) / 4;
    uint32_t previous = _counter(timer);
    uint32_t wraps = 0;

    /**
     * Window [1/2, 3/4) of the period: far from the boundary, where the
     * critical task wakes up (within a few us either side), and after the
     * DMA write of the period. From inside the window the
     * wait returns at once, from just before the boundary it waits for the
     * next period's window.
     */
    for (uint32_t loops = 0; loops < HRTIM_PERIOD_WAIT_MAX_LOOPS; loops++)
    {
        uint32_t now = _counter(timer);
        if (now >= low && now < high)
            break;
        if (now < previous && ++wraps > HRTIM_PERIOD_WAIT_MAX_WRAPS)
            break;
        previous = now;
    }
}

void hrtim_wait_period_end(hrtim_tu_number_t tu_number, uint32_t margin_ns)
{
    hrtim_tu_t timer = tu_channel[tu_number]->pwm_conf.pwm_tu;
    uint32_t period = tu_channel[tu_number]->pwm_conf.period;

    /**
     * Counter ticks in margin_ns, from the HRTIM clock: f_HRTIM x 32 counts
     * per second, divided by 2^ckpsc. (pwm_conf.resolution is never filled
     * in; pwm_conf.frequency did not follow at 200 kHz, the margin came out
     * ~4x too short.)
     */
    uint64_t counts_per_s = ((uint64_t)hrtim_get_apb2_clock() * 32U) >>
                            tu_channel[tu_number]->pwm_conf.ckpsc;
    uint32_t margin = (uint32_t)(((uint64_t)margin_ns * counts_per_s) /
                                 1000000000ULL);
    uint32_t limit = (margin < period) ? (period - margin) : 0;
    uint32_t previous = _counter(timer);
    uint32_t wraps = 0;

    for (uint32_t loops = 0; loops < HRTIM_PERIOD_WAIT_MAX_LOOPS; loops++)
    {
        uint32_t now = _counter(timer);
        if (now >= limit)
            break;
        if (now < previous && ++wraps >= HRTIM_PERIOD_WAIT_MAX_WRAPS)
            break;
        previous = now;
    }
}

#if DT_NODE_HAS_STATUS(DT_NODELABEL(dma2), okay)

/**
 *  HAL override, same as Rs485.cpp: Zephyr only routes the channel IRQ to
 *  our callback, the channel itself is configured with LL.
 */
#define STM32_DMA_HAL_OVERRIDE 0x7F

/* Timing unit x uses DMA2 channel x+1 (LL channel index x) */
#define HRTIM_DMA          DMA2
#define HRTIM_DMA_REQ_BASE LL_DMAMUX_REQ_HRTIM1_A


typedef struct
{
    uint32_t *buffer;
    uint16_t length;
    bool initialized;
    bool running;
    bool irq_routed;
    hrtim_dma_cmp1_callback_t callback;
    volatile uint32_t error_count;
} hrtim_dma_ctx_t;

static const struct device *dma2_dev = DEVICE_DT_GET(DT_NODELABEL(dma2));

static hrtim_dma_ctx_t dma_ctx[HRTIM_STU_NUMOF];

/* Private functions */

static inline uint32_t _ll_channel(hrtim_tu_number_t tu_number)
{
    /* LL_DMA_CHANNEL_1 .. LL_DMA_CHANNEL_8 are 0 .. 7 on STM32G4 */
    return (uint32_t)tu_number;
}

/* DMA2 channel x+1 interrupt of timing unit x (not contiguous after 5) */
static const IRQn_Type dma_irq[HRTIM_STU_NUMOF] = {
    DMA2_Channel1_IRQn, DMA2_Channel2_IRQn, DMA2_Channel3_IRQn,
    DMA2_Channel4_IRQn, DMA2_Channel5_IRQn, DMA2_Channel6_IRQn,
};

/**
 * Clears the channel flags and its pending interrupt. Clearing only the
 * flags left the NVIC request pending: the DMA interrupt then ran with no
 * flag set, Zephyr logged "Transfer Error" from it, and the log output
 * delayed the critical task by ~70 us.
 */
static inline void _clear_flags(hrtim_tu_number_t tu_number)
{
    /* Each channel owns 4 flag bits in IFCR, CGIF clears all of them */
    HRTIM_DMA->IFCR = (DMA_IFCR_CGIF1 << (4U * (uint32_t)tu_number));
    NVIC_ClearPendingIRQ(dma_irq[tu_number]);
}

static void _dma_callback(const struct device *dev,
                          void *user_data,
                          uint32_t channel,
                          int status)
{
    hrtim_tu_number_t tu_number = (hrtim_tu_number_t)(uintptr_t)user_data;
    hrtim_dma_ctx_t *ctx = &dma_ctx[tu_number];
    uint32_t flag_shift = 4U * (uint32_t)tu_number;

    /**
     * With HAL override, Zephyr leaves the half / complete flags set, the
     * channel interrupt would fire again forever and starve every thread.
     * Clear only the flag being handled: if both are set, the interrupt
     * comes back once for the other one.
     */
    if (status == DMA_STATUS_BLOCK)
    {
        HRTIM_DMA->IFCR = (DMA_IFCR_CHTIF1 << flag_shift);
    }
    else if (status == DMA_STATUS_COMPLETE)
    {
        HRTIM_DMA->IFCR = (DMA_IFCR_CTCIF1 << flag_shift);
    }
    else
    {
        HRTIM_DMA->IFCR = (DMA_IFCR_CGIF1 << flag_shift);
    }

    if (status < 0)
    {
        ctx->error_count++;
        return;
    }

    if (ctx->callback == NULL)
    {
        return;
    }

    /* Half transfer: first half has just been read, otherwise second half */
    uint8_t finished_half = (status == DMA_STATUS_BLOCK) ? 0 : 1;
    ctx->callback(tu_number, finished_half);
}

/* Public API */

int8_t hrtim_dma_cmp1_init(hrtim_tu_number_t tu_number,
                           uint32_t *buffer,
                           uint16_t length,
                           hrtim_dma_cmp1_callback_t callback)
{
    if (tu_number >= HRTIM_STU_NUMOF || buffer == NULL || length == 0)
        return -1;

    if (dma_ctx[tu_number].running)
        return -1;

    if (!device_is_ready(dma2_dev))
        return -1;

    /* One period = one repetition event only holds for up-counting */
    if (hrtim_get_modulation(tu_number) != Lft_aligned)
        return -1;

    hrtim_dma_ctx_t *ctx = &dma_ctx[tu_number];
    uint32_t channel = _ll_channel(tu_number);
    hrtim_tu_t timer = tu_channel[tu_number]->pwm_conf.pwm_tu;

    /**
     * Route the channel IRQ to our callback once: with HAL override Zephyr
     * keeps the stream marked busy, a second dma_config() is refused with
     * "dma stream x is busy" (callback and user data never change anyway).
     */
    if (!ctx->irq_routed)
    {
        struct dma_config dma_config_s = {0};
        dma_config_s.dma_callback = _dma_callback;
        dma_config_s.user_data = (void *)(uintptr_t)tu_number;
        dma_config_s.linked_channel = STM32_DMA_HAL_OVERRIDE;
        if (dma_config(dma2_dev, channel + 1, &dma_config_s) != 0)
            return -1;
        ctx->irq_routed = true;
    }

    LL_DMA_DisableChannel(HRTIM_DMA, channel);

    LL_DMA_ConfigTransfer(HRTIM_DMA, channel,
                          LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                          LL_DMA_MODE_CIRCULAR |
                          LL_DMA_PERIPH_NOINCREMENT |
                          LL_DMA_MEMORY_INCREMENT |
                          LL_DMA_PDATAALIGN_WORD |
                          LL_DMA_MDATAALIGN_WORD |
                          LL_DMA_PRIORITY_VERYHIGH);

    LL_DMA_SetPeriphAddress(HRTIM_DMA, channel,
            (uint32_t)&(HRTIM1->sTimerxRegs[tu_number].CMP1xR));
    LL_DMA_SetMemoryAddress(HRTIM_DMA, channel, (uint32_t)buffer);
    LL_DMA_SetDataLength(HRTIM_DMA, channel, length);
    LL_DMA_SetPeriphRequest(HRTIM_DMA, channel,
                            HRTIM_DMA_REQ_BASE + (uint32_t)tu_number);

    _clear_flags(tu_number);

    /* Transfer errors are always reported, half/complete only on request */
    LL_DMA_EnableIT_TE(HRTIM_DMA, channel);
    if (callback != NULL)
    {
        LL_DMA_EnableIT_HT(HRTIM_DMA, channel);
        LL_DMA_EnableIT_TC(HRTIM_DMA, channel);
    }
    else
    {
        LL_DMA_DisableIT_HT(HRTIM_DMA, channel);
        LL_DMA_DisableIT_TC(HRTIM_DMA, channel);
    }

    /* Make sure no other HRTIM event of this timer requests the channel */
    LL_HRTIM_DisableDMAReq_REP(HRTIM1, timer);

    ctx->buffer = buffer;
    ctx->length = length;
    ctx->callback = callback;
    ctx->error_count = 0;
    ctx->initialized = true;

    return 0;
}

int8_t hrtim_dma_cmp1_start(const hrtim_tu_number_t *tu_numbers, uint8_t count)
{
    if (tu_numbers == NULL || count == 0)
        return -1;

    for (uint8_t i = 0; i < count; i++)
    {
        hrtim_tu_number_t tu = tu_numbers[i];
        if (tu >= HRTIM_STU_NUMOF || !dma_ctx[tu].initialized ||
            dma_ctx[tu].running)
            return -1;
    }

    /* Arm the channels: nothing moves until the REP requests are enabled */
    for (uint8_t i = 0; i < count; i++)
    {
        hrtim_tu_number_t tu = tu_numbers[i];
        uint32_t channel = _ll_channel(tu);

        /**
         * A leftover output swap (used by setDutyCycle for 100 %) would
         * invert the whole sequence. Swapping back restores the switch
         * convention of the timing unit.
         */
        if (tu_channel[tu]->pwm_conf.duty_swap)
            hrtim_output_hot_swap(tu);

        LL_DMA_DisableChannel(HRTIM_DMA, channel);
        LL_DMA_SetMemoryAddress(HRTIM_DMA, channel,
                                (uint32_t)dma_ctx[tu].buffer);
        LL_DMA_SetDataLength(HRTIM_DMA, channel, dma_ctx[tu].length);
        _clear_flags(tu);
        dma_ctx[tu].error_count = 0;
        LL_DMA_EnableIT_TE(HRTIM_DMA, channel);
        if (dma_ctx[tu].callback != NULL)
        {
            LL_DMA_EnableIT_HT(HRTIM_DMA, channel);
            LL_DMA_EnableIT_TC(HRTIM_DMA, channel);
        }
        LL_DMA_EnableChannel(HRTIM_DMA, channel);
    }

    /**
     * Enable the requests in the middle of a period of the first timing
     * unit (hrtim_wait_period_middle), so that the first transfer always
     * happens on the same repetition event, wherever the call falls within
     * a quarter period around a boundary. All timing units share the
     * master period. The REP flags are cleared before enabling the
     * requests, so a flag left set earlier cannot cause an early transfer.
     */
    unsigned int key = irq_lock();

    hrtim_wait_period_middle(tu_numbers[0]);

    for (uint8_t i = 0; i < count; i++)
    {
        hrtim_tu_number_t tu = tu_numbers[i];
        hrtim_tu_t timer = tu_channel[tu]->pwm_conf.pwm_tu;
        LL_HRTIM_ClearFlag_REP(HRTIM1, timer);
        LL_HRTIM_EnableDMAReq_REP(HRTIM1, timer);
        dma_ctx[tu].running = true;
    }

    irq_unlock(key);

    return 0;
}

void hrtim_dma_cmp1_stop(hrtim_tu_number_t tu_number)
{
    if (tu_number >= HRTIM_STU_NUMOF || !dma_ctx[tu_number].running)
        return;

    /* Stop the requests first, then the channel interrupts and the channel */
    uint32_t channel = _ll_channel(tu_number);
    LL_HRTIM_DisableDMAReq_REP(HRTIM1, tu_channel[tu_number]->pwm_conf.pwm_tu);
    LL_DMA_DisableIT_HT(HRTIM_DMA, channel);
    LL_DMA_DisableIT_TC(HRTIM_DMA, channel);
    LL_DMA_DisableIT_TE(HRTIM_DMA, channel);
    LL_DMA_DisableChannel(HRTIM_DMA, channel);
    _clear_flags(tu_number);

    dma_ctx[tu_number].running = false;
}

bool hrtim_dma_cmp1_is_running(hrtim_tu_number_t tu_number)
{
    if (tu_number >= HRTIM_STU_NUMOF)
        return false;

    return dma_ctx[tu_number].running;
}

uint16_t hrtim_dma_cmp1_get_next_index(hrtim_tu_number_t tu_number)
{
    if (tu_number >= HRTIM_STU_NUMOF || !dma_ctx[tu_number].initialized)
        return 0;

    uint16_t remaining = LL_DMA_GetDataLength(HRTIM_DMA,
                                              _ll_channel(tu_number));

    return (dma_ctx[tu_number].length - remaining) % dma_ctx[tu_number].length;
}

uint32_t hrtim_dma_cmp1_get_error_count(hrtim_tu_number_t tu_number)
{
    if (tu_number >= HRTIM_STU_NUMOF)
        return 0;

    return dma_ctx[tu_number].error_count;
}

#else /* dma2 not enabled */

int8_t hrtim_dma_cmp1_init(hrtim_tu_number_t tu_number,
                           uint32_t *buffer,
                           uint16_t length,
                           hrtim_dma_cmp1_callback_t callback)
{
    return -1;
}

int8_t hrtim_dma_cmp1_start(const hrtim_tu_number_t *tu_numbers, uint8_t count)
{
    return -1;
}

void hrtim_dma_cmp1_stop(hrtim_tu_number_t tu_number)
{
}

bool hrtim_dma_cmp1_is_running(hrtim_tu_number_t tu_number)
{
    return false;
}

uint16_t hrtim_dma_cmp1_get_next_index(hrtim_tu_number_t tu_number)
{
    return 0;
}

uint32_t hrtim_dma_cmp1_get_error_count(hrtim_tu_number_t tu_number)
{
    return 0;
}

#endif /* DT_NODE_HAS_STATUS(DT_NODELABEL(dma2), okay) */
