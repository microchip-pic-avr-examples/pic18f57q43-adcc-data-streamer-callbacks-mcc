 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
� [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "mcc_generated_files/system/system.h"

static void ADCC_ConversionDone_Callback(void);
static void Timer_Callback_100ms(void);

static const struct TMR_INTERFACE *Timer_dsFrame = &Timer_ds; // TODO: Replace TimerX with name of const struct TMR_INTERFACE, from MCC Generated Files > timer > tmrx.c 
static const struct TMR_INTERFACE *Timer_printfADC = &Timer_printf;  

static volatile bool sendFrame = false;
static volatile bool printResult = false;

void ADCC_ConversionDone_Callback(void)
{
    DataStreamer.adcResult = ADCC_GetConversionResult();
    DataStreamer.adcSampleCount++;
    IO_LED_Toggle();
    IO_Debug_Toggle();
    sendFrame = true;
}

void Timer_Callback_100ms(void)
{
    ADCC_DischargeSampleCapacitor(); 
    ADCC_StartConversion(channel_ANA7); // TODO: Replace x with the ADCC Channel you selected in Pin Grid View
}

void Timer_Callback_1s(void)
{
    printResult = true;
}


int main(void)
{
    SYSTEM_Initialize();

    DataStreamer.adcSampleCount = 0;
    DataStreamer.adcResult = 0;
    ADCC_SetADIInterruptHandler(ADCC_ConversionDone_Callback);
    
    Timer_dsFrame->TimeoutCallbackRegister(Timer_Callback_100ms);
    Timer_printfADC->TimeoutCallbackRegister(Timer_Callback_1s);
    
    INTERRUPT_GlobalInterruptEnable();

    while (1)
    {
        if (sendFrame)
        {
            WriteFrame();
            sendFrame = false;
        }
        if (printResult)
        {
            (void) printf("The ADC Result is: %d\r\n", DataStreamer.adcResult);
            IO_Debug_SetHigh();
            printResult = false;
        }
    }
}