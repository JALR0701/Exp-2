

/**
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

int32_t color = 0, R = 1, G = 0, B = 0;

int main(void){

    uint32_t periodo;
    int freq = 1; //Hz

    //Establece el reloj del microcontrolador
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    //Habilita el periferico GPIOF y GPIOA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //Habilita pines de salida (LEDS R, B, G respectivamente)
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    //Configurar Timer0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    periodo = (SysCtlClockGet()/freq)/2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, periodo -1);

    //Configurar UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //Habilitar interrupciones//
    //TIMER 0
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    IntMasterEnable();

    //UART
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);
    //*****************************//

    //Habilitar timer
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1){
    }
}

void Timer0IntHandler(void){
    //Limpiar la interrupcion del timer
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //rutina
    if (R == 1){
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
        }else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        }
    }
    if (G == 1){
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }
    }
    if (B == 1){
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
        }else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
     }
}

void UARTIntHandler(void)
{
    uint32_t ui32Status;
    //Verificamos la Interrupción del UART
    ui32Status = UARTIntStatus(UART0_BASE, true);
    //Limpiamos la interrupción
    UARTIntClear(UART0_BASE, ui32Status);
    //Leemos valores
    while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
    {
        color = UARTCharGet(UART0_BASE);

        if (color == 82 && R == 0){
            R = 1;
        }else if (color == 82 && R == 1){
            R = 0;
        }

        if (color == 71 && G == 0){
            G = 1;
        }else if (color == 71 && G == 1){
            G = 0;
        }

        if (color == 66 && B == 0){
            B = 1;
        }else if (color == 66 && B == 1){
            B = 0;
        }
    }
}
