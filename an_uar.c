#include "driverlib.h"

uint8_t RXData = 0, TXData = 0;
uint8_t RXStatus = 0;


void Gpio_Init(void);
void Uart_Initialization(void);
void Uart_Send_Timeout(uint8_t Data, uint16_t Timeout);

void Uart_Receive_Timeout(uint8_t Data, uint16_t Timeout);


void main(void)
{
	WDT_A_hold(WDT_A_BASE);

	Gpio_Init()

	PMM_unlockLPM5();
	
	Uart_Initialization();

	// enable interrupt
	__enable_interrupt();

	while(1)
	{
		//
	}

}


void Gpio_Init(void)
{
	// Uart pins
	GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
											   GPIO_PIN0 + GPIO_PIN1,
											   GPIO_PRIMARY_MODULE_FUNCTION);
}

void Uart_Initialization(void)
{
	EUSCI_A_UART_initParam paramUart = {0};
	paramUart.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
	paramUart.clockPrescalar = 109;
	paramUart.firstModReg = 0;
	paramUart.secondModReg = 1;
	paramUart.parity = EUSCI_A_UART_NO_PARITY;
	paramUart.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
	paramUart.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
	paramUart.uartMode = EUSCI_A_UART_MODE;
	paramUart.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;

    if(STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &paramUart))
    {
        return;
    }

    //Enable UART module for operation
    EUSCI_A_UART_enable(EUSCI_A0_BASE);	//~(UCSWRST)

    //Enable Transmit Interrupt
    //EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT);
    //EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,EUSCI_A_UART_TRANSMIT_INTERRUPT);
    
    // Clear RX int. flag
    EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    // Enable USCI_A0 RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);      // Enable interrupt
}//

void Uart_Send_Timeout(uint8_t Data, uint16_t Timeout)
{
	do
	{
		UCA0TXBUF = Data;
		//EUSCI_A_UART_transmitData(EUSCI_A0_BASE, Data)
		/* code */
	} while (Timeout--);
}


void Uart_Send_Timeout2(uint8_t Data, uint16_t Timeout)
{

}



#pragma vector=USCI_A0_VECTOR
__interrupt void EUSCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        RXData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);

        RXStatus = 1;
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    }
}
