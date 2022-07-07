/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * @brief    A project template for M031 MCU.
 *
 * Copyright (C) 2017 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NuMicro.h"


// #define USE_TIMER_PWM
#define USE_PWM_ONLY
// #define GPIO_DEMO

uint8_t DUTY_LED = 0;
uint8_t FLAG_LED = 1;
uint8_t CNT_LED = 0;	


void PWM_Set_Duty(uint16_t inputduty)
{
    // uint32_t u32NewCMR = 0;

	uint16_t Duty = inputduty; 
	
	Duty = (inputduty >= 100)?(100):(inputduty);

	#if 1
	PWM_ConfigOutputChannel(PWM0, 4, 10000, Duty);
	#else
    u32NewCMR = PWM_GET_CNR(PWM0, 4);
    u32NewCMR = u32NewCMR * (100-Duty)/100;	//u32NewCMR = (u32NewCMR+1) * (Duty)/100;
	PWM_SET_CMR(PWM0, 4, u32NewCMR);
	#endif
}

void PWM_Init(void)	//PB1 , PWM0_CH4
{
    PWM_ConfigOutputChannel(PWM0, 4, 10000, 0);
    PWM_EnableOutput(PWM0, PWM_CH_4_MASK);
  	PWM_SET_OUTPUT_LEVEL(PWM0, PWM_CH_4_MASK, PWM_OUTPUT_NOTHING, PWM_OUTPUT_NOTHING, PWM_OUTPUT_HIGH, PWM_OUTPUT_LOW);

    /* Start PWM module */
    PWM_Start(PWM0, PWM_CH_4_MASK);
}

void TMR1_IRQHandler(void)
{
	// static uint32_t LOG = 0;
	static uint16_t CNT = 0;
	static uint8_t flag = 0;

    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
	
		if (CNT++ >= 10000)
		{		
			CNT = 0;

			flag = !flag;
			

			if (flag)
			{
				/* Unlock protected registers */
				SYS_UnlockReg();
				CLK_EnableModuleClock(PWM0_MODULE);
				CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, 0);	
				SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_PWM0_CH4;
				/* Lock protected registers */
				SYS_LockReg();

				PWM_Init();
				PWM_Set_Duty(50);
			}
			else
			{		
				PWM_Set_Duty(0);	
				/* Start PWM module */
				PWM_Stop(PWM0, PWM_CH_4_MASK);

				PWM_DisableOutput(PWM0, PWM_CH_4_MASK);				

				SYS_UnlockReg();
				CLK_DisableModuleClock(PWM0_MODULE);
				SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_GPIO;
				/* Lock protected registers */
				SYS_LockReg();
			}


			// #if defined (GPIO_DEMO)
			// PB1 = !PB1 ;
			// #endif
			
        	// printf("%s : %4d\r\n",__FUNCTION__,LOG++);
		}
    }
}


void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void GPIO_Init(void)	//PB1
{
	GPIO_SetMode(PB,BIT1,GPIO_MODE_OUTPUT);
}

void UART0_Init(void)
{

    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);

	/* Set UART receive time-out */
	UART_SetTimeoutCnt(UART0, 20);

	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());
	
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC clock (Internal RC 48MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Wait for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Select HCLK clock source as HIRC and HCLK source divider as 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

	#if defined (USE_TIMER_PWM)
    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);

    CLK_EnableModuleClock(PWM0_MODULE);
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, 0);	

    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_PWM0_CH4;
	#endif

	#if defined (USE_PWM_ONLY)	
    CLK_EnableModuleClock(PWM0_MODULE);
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL2_PWM0SEL_PCLK0, 0);	

    SYS->GPB_MFPL = (SYS->GPB_MFPL & (~SYS_GPB_MFPL_PB1MFP_Msk)) | SYS_GPB_MFPL_PB1MFP_PWM0_CH4;

	#endif
	
    /* Enable UART0 clock */
    // CLK_EnableModuleClock(UART0_MODULE);
    /* Switch UART0 clock source to HIRC */
    // CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_PCLK0, CLK_CLKDIV0_UART0(1));
	
    /* Set PB multi-function pins for UART0 RXD=PB.12 and TXD=PB.13 */
    // SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk))    |       \
    //                 (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
	
    /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for M031 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    SYS_Init();

    // UART0_Init();

	#if defined (USE_TIMER_PWM)
	TIMER1_Init();
	#endif

	// #if defined (GPIO_DEMO)
	// GPIO_Init();
	// #endif
	
	#if defined (USE_PWM_ONLY)
	PWM_Init();
	PWM_Set_Duty(50);
	#endif
	
    /* Got no where to go, just loop forever */
    while(1)
    {

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
