/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /** Requirement
48 MHz IRC in (1) Stop and (2) Vlps mode? 
Conditions:
---------------------
Stop
a. Baseline
b.  (a.) + HIRCLPEN + HIRCEN
---------------------
VLPS
a.  Baseline
b.  (a) + BGEN + BGBE + VLPO
c.  (b) + HIRCEN + HIRCLPEN
Updated the "MCG-Lite in Low-power mode" section: "As internal voltage regulator 
is required in HIRC logic, BGEN, BGBE
and VLPO bits in the PMC_REGSC register should also be set before entering VLPR, 
while LIRC can keep working"
*/
/*******************************************************************************
 * Includes Headers
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_lpuart_hal.h"
#include "fsl_pmc_hal.h"
#include "fsl_smc_hal.h"
#include "fsl_mcglite_hal.h"
#include "fpt_mcglite_board_clock_conf.h"
#include "fpt_power_management.h"
#include "fsl_tpm_hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define NUM_OF_POWER_CONF       (8U)
#define NUM_OF_TEST_MODE                (3U)
#define NUM_OF_PRESCALER_MODE         (3U)
#define DEBUG
/*******************************************************************************
 * Global Variables
 ******************************************************************************/
mcglite_power_conf_t * gMeasure_power_conf[] = {  &wait_irc48_48, &wait_irc48_24,
        &vlps_irc8_8,   &vlps_irc2_2, &vlps,
        &stop_irc8_8,   &stop_irc2_2,  &stop
                                               };
int8_t gMeasure_power_index = 0;             // Run or VLPS
tpm_pwm_param_t param = {
    .mode              = kTpmEdgeAlignedPWM,
    .edgeMode          = kTpmHighTrue,
    .uFrequencyHZ      = NULL,
    .uDutyCyclePercent = NULL
};

pmc_bandgap_buffer_config_t pmc_bg = {
    .enable = true,
    .enableInLowPower =  true
};
static char * test_mode_str[] = {"","Base line", "ClockSource Enable","Run"};
typedef enum _test_mode {
    Null,
    Baseline,
    Enable,
    Run
} test_mode_t;
uint8_t TESTMODE;
static char * tpm_prescaler_str[] = {"null","div-by-1", "div-by-16","div-by-128"};
static tpm_clock_ps_t tpm_prescaler_val[] = {(kTpmDividedBy1),(kTpmDividedBy1), (kTpmDividedBy16), (kTpmDividedBy128)};
uint8_t tpm_prescaler_index = 0;
oscer_config_t config_oscer = {
    .enable = true,   /*!< OSCERCLK enable or not.              */
    .enableInStop = true /*!< OSCERCLK enable or not in stop mode. */
};
int32_t temp;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#ifdef DEBUG
/* Array of mcglite mode string */
static char * mcglite_mode_str[] = {"HIRC48M", "LIRC8M", "LIRC2M", "EXT", "STOP", "Error"};
void print_clk_and_mcg(void);

#endif


/* Main function */
void main(void)
{
    int32_t tmp;
    mcglite_power_conf_t *ppower_conf;
    /* not use DMA */
    CLOCK_SYS_DisableDmaClock(0);
    CLOCK_SYS_SetTpmSrc(0, kClockTpmSrcNone);
    /* enable clock for PORTs */
    CLOCK_SYS_EnablePortClock(PORTA_IDX);
    /* Debug uart */
    configure_lpuart_pins(BOARD_DEBUG_UART_INSTANCE);
    CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcMcgIrClk);
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUD, kDebugConsoleLPUART);

    /**** NOTE *****
     * Before characterization, only enable required clocks (either HIRC or LIRC
     * or none of them) and turn off un-necessary clocks.
     * It can be done via MCG_C1 and MCG_MC.
    */
    printf("Input power mode: \r\n");
    for (tmp = 0; tmp < NUM_OF_POWER_CONF; tmp++) {
        printf("\t%2d: %s\r\n", (tmp + 1), gMeasure_power_conf[tmp]->pdesc_string);
    }
    printf("\tPower mode: ");
    scanf("%d", &tmp);
    gMeasure_power_index = tmp;
    printf("%d\r\n", gMeasure_power_index);

    // Print the initial banner
    printf("\r\n TPM0 CH0 Characterization\n\n\r");
    printf("\r\tInput test mode:\r\n");
    for (int i = 1; i <= NUM_OF_TEST_MODE; i++) {
        printf("\t%2d: %s\r\n", i, test_mode_str[i]);
    }
    printf("\n\r\tTest mode: ");
    scanf("%d", &temp);
    TESTMODE = temp;
    printf ("\t%d \r\n",TESTMODE);
    if (TESTMODE > Enable) {
        printf("\r\tSet PRESCALER mode:\r\n");
        for (int i = 1; i <= NUM_OF_PRESCALER_MODE; i++) {
            printf ("\t%d : %s \r\n",i,tpm_prescaler_str[i]);
        }

        printf("\r\tPRESCALER mode: ");
        scanf("%d", &temp);
        tpm_prescaler_index = temp;
        printf ("\t%d \r\n",tpm_prescaler_index);
    }
    if (TESTMODE==Baseline) printf ("BASELINE\n\r");
    else if (TESTMODE==Enable) printf ("Clock Source Enable\n\r");
    else printf ("RUN\n\r");

#ifdef DEBUG
    printf ("Debugging !\n\r");
#else
    printf ("Ready to measure !\n\r");
#endif

    ppower_conf = gMeasure_power_conf[gMeasure_power_index - 1];

    /* Setup clock */
    if (ppower_conf->pclock_conf != NULL) {
        setup_clk(ppower_conf->pclock_conf);
    }
    if (gMeasure_power_index <=2)
        CLOCK_HAL_SetLircCmd(MCG,  false);
    /**************************************************************************
        Baseline
    ***************************************************************************/
    if (TESTMODE==Baseline) goto MAINLOOP;
    /**************************************************************************
        Enable Clock Source only
    ***************************************************************************/
    /* set tmp clock source*/
    switch (gMeasure_power_index) {
    case 1 : //48 - 24
    case 2 :   // 24 - 12
        CLOCK_SYS_SetTpmSrc(0, kClockTpmSrcIrc48M);
#ifdef DEBUG
        CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcIrc48M);
#endif
        break;
    case 3 : //vlps 8_8
    case 4 : // vlp 2_2
    case 6 : //stop 8_8
    case 7 : //stop 2_2
        CLOCK_SYS_SetTpmSrc(0, kClockTpmSrcMcgIrClk);
        CLOCK_HAL_SetLircStopCmd(MCG,  true);
#ifdef DEBUG
        CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcMcgIrClk);
#endif
        break;
    case 5 : //vlps
    case 8: //stop
        CLOCK_SYS_SetTpmSrc(0, kClockTpmSrcOsc0erClk);
        BOARD_InitOsc0();
        OSC_HAL_SetExternalRefClkCmd(OSC0,true);
        OSC_HAL_SetExternalRefClkInStopModeCmd(OSC0,true);
#ifdef DEBUG
        CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcMcgIrClk);
#endif
        break;
    default:
        break;
    }
    if (TESTMODE==Enable) goto MAINLOOP;
    /**************************************************************************
        Enable Clock Source + module clock
    ***************************************************************************/
    /*Enable TPM clock*/
    CLOCK_SYS_EnableTpmClock(0);
    /**************************************************************************
        Configure module and Run
    ***************************************************************************/
// configure TPM0_CH0 pin
    PORT_HAL_SetMuxMode(PORTA,5u,kPortMuxAlt3);//TPM0_CH2 - PTA5 alt3
// Init TPM.
    TPM_HAL_Reset(TPM0, 2); //TPM0_CH2
    /*trigger mode*/
    TPM_HAL_SetTriggerMode(TPM0,false);
    TPM_HAL_SetStopOnOverflowMode(TPM0, false);
    TPM_HAL_SetReloadOnTriggerMode(TPM0, false);
    /*trigger source*/
    TPM_HAL_SetTriggerSrc(TPM0, kTpmTrigSel0);
    /*global time base*/
    TPM_HAL_EnableGlobalTimeBase(TPM0, false);
    /*Debug mode*/
    TPM_HAL_SetDbgMode(TPM0, false);
    //TPM_HAL_SetWaitMode(TPM0, false);

// Set clock for TPM.
    /*Clock prescaler*/
    TPM_HAL_SetClockDiv(TPM0, tpm_prescaler_val[tpm_prescaler_index]);
    TPM_HAL_DisableChn(TPM0, 2);
    TPM_HAL_SetMod(TPM0, 0xFFFF);
    TPM_HAL_SetChnCountVal(TPM0, 2, 0xFFFE);
    /* Set the requested PWM mode */
    TPM_HAL_EnablePwmMode(TPM0, &param, 2);

    /* Set the TPM clock */
    TPM_HAL_SetClockMode(TPM0, kTpmClockSourceModuleClk);

#ifdef DEBUG
    print_clk_and_mcg();
    printf("\n\rCore clock = %d\n\r",CLOCK_SYS_GetCoreClockFreq());
    printf("\n\rBus clock = %d\n\r",CLOCK_SYS_GetBusClockFreq());
    printf("\n\rTPM clock = %d\n\r",CLOCK_SYS_GetTpmFreq(0));
#endif


MAINLOOP:
#ifndef DEBUG
    DbgConsole_DeInit();
    /* PORTA_PCR1 */
    PORT_HAL_SetMuxMode(PORTA,1u,kPortPinDisabled);
    /* PORTA_PCR2 */
    PORT_HAL_SetMuxMode(PORTA,2u,kPortPinDisabled);

#endif
    /* Setup power mode */
    switch (ppower_conf->power_mode) {

    case kPowerModeVlpr:
    case kPowerModeVlpw:
        /* Disable HIRC */
        CLOCK_HAL_SetHircCmd(MCG, false);
    case kPowerModeWait:
    case kPowerModeStop:
    case kPowerModeVlps:
    case kPowerModeVlls:
        set_power_mode(ppower_conf);
        break;
    default:
        break;
    }

}

#ifdef DEBUG
void print_clk_and_mcg(void)
{
    uint32_t mcgpclk, mcgoutclk, mcgirclk, lpuart_clk;
    mcglite_mode_t mode = CLOCK_HAL_GetMode(MCG);
    mcgpclk = CLOCK_HAL_GetPeripheralClk(MCG);
    mcgirclk = CLOCK_HAL_GetInternalRefClk(MCG);
    mcgoutclk = CLOCK_HAL_GetOutClk(MCG);
    /* Re-set LPUART baudrate */
    lpuart_clk = CLOCK_SYS_GetLpuartFreq(BOARD_DEBUG_UART_INSTANCE);
    LPUART_HAL_SetBaudRate(LPUART0, lpuart_clk, BOARD_DEBUG_UART_BAUD);
    printf("MCG mode: %s, peripheral clk: %u, irclk: %u, out clk: %u\r\n",
           mcglite_mode_str[mode], mcgpclk, mcgirclk, mcgoutclk);
}
#endif
