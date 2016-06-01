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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fsl_device_registers.h"
#include "fsl_uart_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fpt_clock_board_conf.h"
#include "fpt_clock_conf_type.h"
#include "fpt_mcg_mode.h"
#include "fpt_power_management.h"
#include "fsl_dmamux_hal.h"
#include "fsl_edma_hal.h"
#include "fsl_dspi_hal.h"
#include "fsl_port_hal.h"

#include "spi_edma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define noDEBUG
#define TEST_SPI0
#define TRANSMIT

#define NUM_OF_POWER_CONF       		(8U)
#define NUM_OF_BAUDRATE_MODE            (5U)
#define NUM_OF_TEST_MODE                (3U)

#define BASELINE_MODE                   (1U)
#define INIT_MODE                       (2U)
#define RUN_MODE                        (3U)

#define SPI_INSTANT                     (0U)
#define SPI_BASE                        (SPI0) 

#define SPI_MASTER_MODE                 (1U)
#define SPI_SLAVE_MODE                  (0U)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
power_conf_t *gMeasure_power_conf[] =  {&run_pee_120, &run_fei_96, &run_fei_48, &run_fei_24,
										&run_fee_48, &run_fee_24, &run_blpe_8, &vlpr_blpi_4
                                       };
static power_conf_t *ppower_conf;

uint32_t gMeasure_mode = 0;
uint32_t gMeasure_power_index = 0;
uint32_t baudrate_ind=0;

uint8_t baudrate_mode_index = 0;
uint8_t test_mode = BASELINE_MODE;

lpet_module_conf_t module_cfg = {0,0};

static char const * const LPET_MODULE_SETTING_CASES_MENU_FOR_VLPR[] =
{
    "                  ",
    " 1:    250000, VLPR",
    " 2:   1000000, VLPR",
    " 3:   2000000, VLPR",
};

static char const * const LPET_MODULE_SETTING_CASES_MENU[] =
{
    "                  ",
    "1:    500000, RUN",
    "2:   1000000, RUN",
    "3:   2000000, RUN",
    "4:   3000000, RUN",
    "5:   6000000, RUN",
    "6:  12000000, RUN",
    "7:  15000000, RUN",
    "8:  24000000, RUN",
    "9:  30000000, RUN",
};
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#ifdef DEBUG
void print_clk_and_mcg(void);
#endif

uint32_t data;
uint32_t source = 'B';
/*******************************************************************************
 * Code
 ******************************************************************************/
#if DEBUG_MODE
uint8_t c='$';
#elif TRANSMIT
uint8_t c='T';
#else   /*receive*/
uint8_t c='R';
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
int main (void)
{

}

void read_measure_config(void)
{
    uint32_t i=0;
    uint32_t temp=0;
    printf("Input test mode (Baseline = %d, INIT = %d, RUN = %d): \r\n", 
            BASELINE_MODE, INIT_MODE, RUN_MODE);
    
    printf("Select Test Mode: ");
    gMeasure_mode = temp;
    scanf("%d", &temp);
    printf("%d\n", gMeasure_mode);

    printf("Select Power mode:\n");
    for (int i = 0; i < NUM_OF_POWER_CONF; i++)
    {
        printf("\t%2d: %s\r\n", (i+1), gMeasure_power_conf[i]->pdesc_string);
    }

    printf("\tPower mode: ");
    scanf("%d", &temp);
    gMeasure_power_index = temp;
    printf("%d\r\n", gMeasure_power_index);

    printf("Select Master (%d) or Slave mode (%d):\r\n ", SPI_MASTER_MODE, SPI_SLAVE_MODE);
    printf("Mode: ");
    scanf("%d", &temp)
    module_cfg.run_mode = temp;
    printf("%d\r\n", module_cfg.mode);

    if (gMeasure_mode == BASELINE_MODE)
    {
        // No configuration infor is needed in baseline mode
        printf("\n\rBaseline");
        return;
    }

    if (module_cfg.run_mode == SPI_MASTER_MODE)
    {
        printf("\rBaudrate: \n");
        if(gMeasure_power_index != 8)
        {
            for(i=1; i<(NUM_OF_BAUDRATE_MODE); i++)
            {
                printf("\t%s\r\n",  LPET_MODULE_SETTING_CASES_MENU[i]);
            }
            printf("\tChoose : ");
            scanf("%d",&baudrate_ind);
            printf("%d\r\n", baudrate_ind);
        
            switch (baudrate_ind)
            {
                case 1 :
                    module_cfg.baud_rate       = 500000U;
                    break;
                case 2 :
                    module_cfg.baud_rate       = 1000000U;
                    break;
                case 3 :
                    module_cfg.baud_rate       = 2000000U;
                    break;
                case 4 :
                    module_cfg.baud_rate       = 3000000U;
                    break;
                case 5 :
                    module_cfg.baud_rate       = 6000000U;
                    break;
                case 6 :
                    module_cfg.baud_rate       = 12000000U;
                    break;
                case 7 :
                    module_cfg.baud_rate       = 15000000U;
                    break;
                case 8 :
                    module_cfg.baud_rate       = 24000000U;
                    break;
                case 9:
                    module_cfg.baud_rate       = 30000000U;
                    break;
                default :
                    break;
            }
        }

        else
        {
            for (i = 1; i < 4; i++)
            {
                printf("%s\r\n",  LPET_MODULE_SETTING_CASES_MENU_FOR_VLPR[i]);
            }
            printf("Choose: \r\n");
            scanf("%d", &baudrate_ind);
            printf("%d\r\n\n", i);
            switch (i)
            {
                case 1 :
                    module_cfg.baud_rate       = 250000U;
                    break;
                case 2 :
                    module_cfg.baud_rate       = 1000000U;
                    break;
                case 3 :
                    module_cfg.baud_rate       = 2000000U;
                    break;
                default :
                    break;
            }

        }

    }

}

void pin_mux(void)
{
    CLOCK_SYS_EnablePortClock(PORTA_IDX)
    PORT_HAL_SetMuxMode(PORTA,14u,kPortMuxAlt2);
    PORT_HAL_SetMuxMode(PORTA,15u,kPortMuxAlt2);
    PORT_HAL_SetMuxMode(PORTA,16u,kPortMuxAlt2);
}

void module_init(void)
{
    /*Enbale pin*/
    pin_mux();
    
    CLOCK_SYS_EnableSpiClock(SPI_INSTANT);
    DSPI_HAL_Enable(SPI_BASE);
    DSPI_HAL_Init(SPI_BASE);
    DSPI_HAL_Enable(SPI_BASE);
    DSPI_HAL_SetPcsPolarityMode( SPI_BASE, kDspiPcs0,kDspiPcs_ActiveLow);

    if (module_cfg.run_mode == SPI_MASTER_MODE)
    {
        DSPI_HAL_SetMasterSlaveMode(SPI_BASE, kDspiMaster);
        DSPI_HAL_SetDelay(SPI_BASE, 0, 0, 1, kDspiPcsToSck)
        module_cfg.baud_rate =  DSPI_HAL_SetBaudRate(SPI_BASE, 0, module_cfg.baud_rate *1000,CLOCK_SYS_GetSpiFreq(SPI_INSTANCE));
    }

    DSPI_HAL_StopTransfer(SPI_BASE);
    if(module_cfg.run_mode == SPI_MASTER_MODE)
        DSPI_HAL_SetTxFifoFillDmaIntMode(SPI_BASE, kDspiGenerateDmaReq, true);
    else
        DSPI_HAL_SetRxFifoDrainDmaIntMode(SPI_BASE, kDspiGenerateDmaReq, true);
}

void print_clk_and_mcg (void)
{
    uint32_t uartSourceClock, clk;
    clk = CLOCK_HAL_GetOutClk (MCG);
    uartSourceClock = CLOCK_SYS_GetUartFreq (BOARD_DEBUG_UART_INSTANCE);
    mcg_modes_t ch = CLOCK_HAL_GetMcgMode (MCG);
    uart_status_t status = UART_HAL_SetBaudRate (BOARD_DEBUG_UART_BASEADDR, uartSourceClock, BOARD_DEBUG_UART_BAUD);
    const char *mcg_mode_str;
    switch (ch) {
    case kMcgModeFEI  :
        mcg_mode_str ="kMcgModeFEI  ";
        break;
    case kMcgModeFBI  :
        mcg_mode_str ="kMcgModeFBI  ";
        break;
    case kMcgModeBLPI :
        mcg_mode_str ="kMcgModeBLPI ";
        break;
    case kMcgModeFEE  :
        mcg_mode_str ="kMcgModeFEE  ";
        break;
    case kMcgModeFBE  :
        mcg_mode_str ="kMcgModeFBE  ";
        break;
    case kMcgModeBLPE :
        mcg_mode_str ="kMcgModeBLPE ";
        break;
    case kMcgModePBE  :
        mcg_mode_str ="kMcgModePBE  ";
        break;
    case kMcgModePEE  :
        mcg_mode_str ="kMcgModePEE  ";
        break;
    case kMcgModeSTOP :
        mcg_mode_str ="kMcgModeSTOP ";
        break;
    case kMcgModeError:
        mcg_mode_str ="kMcgModeError";
        break;
    }
    power_mode_stat_t power_mode = SMC_HAL_GetStat(SMC);
    const char *power_mode_str;
    switch (power_mode) {
    case    kStatRun :
        power_mode_str ="kStatRun" ;
        break;
    case    kStatStop:
        power_mode_str ="kStatStop";
        break;
    case    kStatVlpr:
        power_mode_str ="kStatVlpr";
        break;
    case    kStatVlpw:
        power_mode_str ="kStatVlpw";
        break;
    case    kStatVlps:
        power_mode_str ="kStatVlps";
        break;
    case    kStatLls :
        power_mode_str ="kStatLls ";
        break;
    case    kStatVlls:
        power_mode_str ="kStatVlls";
        break;
    }
    uint32_t core_clock = CLOCK_SYS_GetCoreClockFreq();
    uint32_t bus_clock = CLOCK_SYS_GetBusClockFreq();
    uint32_t flexbus_clock = CLOCK_SYS_GetFlexbusFreq();
    uint32_t flash_clock = CLOCK_SYS_GetFlashClockFreq();
    uint32_t osc0_clock = CLOCK_SYS_GetOsc0ExternalRefClockFreq();
    uint32_t erc32k_clock = CLOCK_SYS_GetExternalRefClock32kFreq();
    uint32_t irc_clock = CLOCK_SYS_GetInternalRefClockFreq();
    uint32_t pllfll_clock = CLOCK_SYS_GetPllFllClockFreq();
    uint32_t ff_clock = CLOCK_SYS_GetFixedFreqClockFreq();
    printf ("\r\n\tMCG out is: %d, MCG mode is: %s : %s \n", clk, mcg_mode_str,power_mode_str);
    printf ("\r\n\tCoreClock : %d\r\n\tBusClock : %d\r\n\tFlexBusClock : %d\r\n\tFlashClock : %d\r\n\tOSCERClock : %d ",core_clock,bus_clock,flexbus_clock,flash_clock,osc0_clock);
    printf ("\r\n\tERC32KClock : %d",erc32k_clock);
    printf ("\r\n\tIRCClock : %d",irc_clock);
    printf ("\r\n\tPLL/FLL/IRC48MClock : %d",pllfll_clock);
    printf ("\r\n\tFFClock : %d\n",ff_clock);
}
/*******************************************************************************
 * EOF
 ******************************************************************************/
