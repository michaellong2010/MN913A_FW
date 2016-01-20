/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2009 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "DrvPWM.h"
#include "DrvSYS.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define PWMA_CNR0        PWMA_BASE + 0x0C
#define PWMA_PDR0        PWMA_BASE + 0x14
#define PWMA_CRLR0       PWMA_BASE + 0x58
#define PWMA_CFLR0       PWMA_BASE + 0x5C

#define PWMB_CNR0        PWMB_BASE + 0x0C
#define PWMB_PDR0        PWMB_BASE + 0x14
#define PWMB_CRLR0       PWMB_BASE + 0x58
#define PWMB_CFLR0       PWMB_BASE + 0x5C

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static S_DRVPWM_CALLBACK_T g_sDrvPWMAHandler = {0};
static S_DRVPWM_CALLBACK_T g_sDrvPWMBHandler = {0};
int16_t g_u16cnrData = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetVersion                                                                             */
/*                                                                                                         */
/* Parameters:        	                                                                                   */
/*               None	                                                         						   */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               The DrvPWM version number                                                                 */
/*                                                                                                         */
/* Description:                                                                                            */
/*               This function is used to get the DrvPWM version number                                    */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPWM_GetVersion(void)
{
	return DRVPWM_VERSION_NUM;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <PWMA_IRQHandler>                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle PWMA interrupt event           		                     		           */
/*---------------------------------------------------------------------------------------------------------*/
void PWMA_IRQHandler(void)
{    
	/* Handle PWMA Timer function */
	if (PWMA->PIIR.PWMIF0)
	{
		PWMA->PIIR.PWMIF0 = 1;		 
		if (g_sDrvPWMAHandler.pfnPWM0CallBack != 0)
		{                           
        	g_sDrvPWMAHandler.pfnPWM0CallBack();
		}	
	}

	if (PWMA->PIIR.PWMIF1)
	{ 
		PWMA->PIIR.PWMIF1 = 1;		 
		if (g_sDrvPWMAHandler.pfnPWM1CallBack != 0)                           
        {
			g_sDrvPWMAHandler.pfnPWM1CallBack();
		}	
	}
 	
	if (PWMA->PIIR.PWMIF2)
	{
		PWMA->PIIR.PWMIF2 = 1;	 
		if (g_sDrvPWMAHandler.pfnPWM2CallBack != 0)                           
        {
			g_sDrvPWMAHandler.pfnPWM2CallBack();
		}	
	}

 	if (PWMA->PIIR.PWMIF3)
	{ 
		PWMA->PIIR.PWMIF3 = 1;	 
		if (g_sDrvPWMAHandler.pfnPWM3CallBack != 0)                           
        {
			g_sDrvPWMAHandler.pfnPWM3CallBack();
		}	
	}

    /* Handle PWMA Capture function */
	if (PWMA->CCR0.CAPIF0) 
    {
        PWMA->CCR0.CAPIF0 = 1;
		if (g_sDrvPWMAHandler.pfnCAP0CallBack != 0)
        {
        	g_sDrvPWMAHandler.pfnCAP0CallBack();
        }
	}
    
	if (PWMA->CCR0.CAPIF1) 
    {
        PWMA->CCR0.CAPIF1 = 1;
		if (g_sDrvPWMAHandler.pfnCAP1CallBack != 0)
        {
        	g_sDrvPWMAHandler.pfnCAP1CallBack();
        }
	}
    
	if (PWMA->CCR1.CAPIF2) 
    {
        PWMA->CCR1.CAPIF2 = 1;
		if (g_sDrvPWMAHandler.pfnCAP2CallBack != 0)
        {
        	g_sDrvPWMAHandler.pfnCAP2CallBack();
        }
	}
        
    if (PWMA->CCR1.CAPIF3) 
    {
        PWMA->CCR1.CAPIF3 = 1;
		if (g_sDrvPWMAHandler.pfnCAP3CallBack != 0)
        {
        	g_sDrvPWMAHandler.pfnCAP3CallBack();
        }
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <PWMB_IRQHandler>                                                                         */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               None                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/*                                                                                                         */
/* Description:                                                                                            */
/*               ISR to handle PWMB interrupt event           		                     		           */
/*---------------------------------------------------------------------------------------------------------*/
void PWMB_IRQHandler(void)
{    
	/* Handle PWMB Timer function */
	if (PWMB->PIIR.PWMIF0)
	{
		PWMB->PIIR.PWMIF0 = 1;		 
		if (g_sDrvPWMBHandler.pfnPWM0CallBack != 0)
		{                           
        	g_sDrvPWMBHandler.pfnPWM0CallBack();
		}	
	}

	if (PWMB->PIIR.PWMIF1)
	{ 
		PWMB->PIIR.PWMIF1 = 1;		 
		if (g_sDrvPWMBHandler.pfnPWM1CallBack != 0)                           
        {
			g_sDrvPWMBHandler.pfnPWM1CallBack();
		}	
	}
 	
	if (PWMB->PIIR.PWMIF2)
	{
		PWMB->PIIR.PWMIF2 = 1;	 
		if (g_sDrvPWMBHandler.pfnPWM2CallBack != 0)                           
        {
			g_sDrvPWMBHandler.pfnPWM2CallBack();
		}	
	}

 	if (PWMB->PIIR.PWMIF3)
	{ 
		PWMB->PIIR.PWMIF3 = 1;	 
		if (g_sDrvPWMBHandler.pfnPWM3CallBack != 0)                           
        {
			g_sDrvPWMBHandler.pfnPWM3CallBack();
		}	
	}

    /* Handle PWMB Capture function */
	if (PWMB->CCR0.CAPIF0) 
    {
        PWMB->CCR0.CAPIF0 = 1;
		if (g_sDrvPWMBHandler.pfnCAP0CallBack != 0)
        {
        	g_sDrvPWMBHandler.pfnCAP0CallBack();
        }
	}
    
	if (PWMB->CCR0.CAPIF1) 
    {
        PWMB->CCR0.CAPIF1 = 1;
		if (g_sDrvPWMBHandler.pfnCAP1CallBack != 0)
        {
        	g_sDrvPWMBHandler.pfnCAP1CallBack();
        }
	}
    
	if (PWMB->CCR1.CAPIF2) 
    {
        PWMB->CCR1.CAPIF2 = 1;
		if (g_sDrvPWMBHandler.pfnCAP2CallBack != 0)
        {
        	g_sDrvPWMBHandler.pfnCAP2CallBack();
        }
	}
        
    if (PWMB->CCR1.CAPIF3) 
    {
        PWMB->CCR1.CAPIF3 = 1;
		if (g_sDrvPWMBHandler.pfnCAP3CallBack != 0)
        {
        	g_sDrvPWMBHandler.pfnCAP3CallBack();
        }
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_IsTimerEnabled                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer   - [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3      */
/*               						DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7      */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               0		disable                                                                            */
/*               1		enable	                                                                           */
/* Description:                                                                                            */
/*               This function is used to get PWMA/PWMB specified timer enable/disable state  	           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPWM_IsTimerEnabled(uint8_t u8Timer)
{
	int32_t status;

	status = 0;
	
	switch (u8Timer)
	{
		case DRVPWM_TIMER0:
			status = PWMA->PCR.CH0EN;
			break;
		case DRVPWM_TIMER1:
			status = PWMA->PCR.CH1EN;				
			break;
		case DRVPWM_TIMER2:
			status = PWMA->PCR.CH2EN;
			break;																	  
		case DRVPWM_TIMER3:
			status = PWMA->PCR.CH3EN;
			break;		
		case DRVPWM_TIMER4:
			status = PWMB->PCR.CH0EN;
			break;
		case DRVPWM_TIMER5:
			status = PWMB->PCR.CH1EN;				
			break;
		case DRVPWM_TIMER6:
			status = PWMB->PCR.CH2EN;
			break;																	  
		case DRVPWM_TIMER7:
			status = PWMB->PCR.CH3EN;
			break;	
	}
	
	return status;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SetTimerCounter                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*              							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*               u16Counter     - [in]      Timer counter : 0~65535                                        */
/* Returns:                                                                                                */
/*               None	                                                                           		   */
/* Description:                                                                                            */
/*               This function is used to set the PWM0~7 specified timer counter 				   		   */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_SetTimerCounter(uint8_t u8Timer, uint16_t u16Counter)
{	
	if (u8Timer >= DRVPWM_TIMER4)
	{
		*((__IO uint32_t *) (PWMB_CNR0 + (u8Timer - DRVPWM_TIMER4) * 12)) = u16Counter;
	}
	else
	{
		*((__IO uint32_t *) (PWMA_CNR0 + (u8Timer & 0x7) * 12)) = u16Counter;	
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetTimerCounter                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*              							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The specified timer counter value                                                         */
/*              											                                        	   */
/* Description:                                                                                            */
/*               This function is used to get the PWM0~7 specified timer counter value 			           */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPWM_GetTimerCounter(uint8_t u8Timer)
{
	uint32_t u32Reg = 0;
	
	if (u8Timer >= DRVPWM_TIMER4)
	{
		u32Reg = *((__IO uint32_t *) (PWMB_PDR0 + (u8Timer - DRVPWM_TIMER4) * 12));
	}
	else
	{
		u32Reg = *((__IO uint32_t *) (PWMA_PDR0 + (u8Timer & 0x7) * 12));	
	}

	return u32Reg;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_EnableInt		                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*              							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   / DRVPWM_CAP2   / DRVPWM_CAP3    */
/*         									DRVPWM_CAP4   / DRVPWM_CAP5   / DRVPWM_CAP6   / DRVPWM_CAP7    */
/*               u8Int	 		- [in]		DRVPWM_CAP_RISING_INT/DRVPWM_CAP_FALLING_INT/DRVPWM_CAP_ALL_INT*/
/*              					        (The parameter is valid only when capture function)	       	   */
/*               pfncallback	- [in]		The call back function for specified timer / capture		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None                                                                            		   */
/*               			                                                                          	   */
/* Description:                                                                                            */
/*               This function is used to Enable the PWM0~7 timer/capture interrupt 		               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_EnableInt(uint8_t u8Timer, uint8_t u8Int, PFN_DRVPWM_CALLBACK pfncallback)
{
	int32_t Rflag, Fflag;	

	Rflag = u8Int & 0x01;
	Fflag = u8Int & 0x02;
		
	switch (u8Timer)
	{	
		case DRVPWM_TIMER0:
			PWMA->PIER.PWMIE0 = 1;
			g_sDrvPWMAHandler.pfnPWM0CallBack = pfncallback;
			break;
		case DRVPWM_CAP0:			
			if (Rflag)
				PWMA->CCR0.CRL_IE0 = 1;
			if (Fflag)
				PWMA->CCR0.CFL_IE0 = 1;
		    g_sDrvPWMAHandler.pfnCAP0CallBack = pfncallback;
			break;	
		case DRVPWM_TIMER1:
			PWMA->PIER.PWMIE1 = 1;
			g_sDrvPWMAHandler.pfnPWM1CallBack = pfncallback;
			break;
		case DRVPWM_CAP1:			
			if (Rflag)
				PWMA->CCR0.CRL_IE1 = 1;
			if (Fflag)
				PWMA->CCR0.CFL_IE1 = 1;
		    g_sDrvPWMAHandler.pfnCAP1CallBack = pfncallback;
			break;
		case DRVPWM_TIMER2:
			PWMA->PIER.PWMIE2 = 1;
			g_sDrvPWMAHandler.pfnPWM2CallBack = pfncallback;
			break;
		case DRVPWM_CAP2:			
			if (Rflag)
				PWMA->CCR1.CRL_IE2 = 1;
			if (Fflag)
				PWMA->CCR1.CFL_IE2 = 1;
		    g_sDrvPWMAHandler.pfnCAP2CallBack = pfncallback;
			break;
		case DRVPWM_TIMER3:
			PWMA->PIER.PWMIE3 = 1;
			g_sDrvPWMAHandler.pfnPWM3CallBack = pfncallback;
			break;
		case DRVPWM_CAP3:			
			if (Rflag)
				PWMA->CCR1.CRL_IE3 = 1;
			if (Fflag)
				PWMA->CCR1.CFL_IE3 = 1;		    
			g_sDrvPWMAHandler.pfnCAP3CallBack = pfncallback;
			break;			
			
		case DRVPWM_TIMER4:
			PWMB->PIER.PWMIE0 = 1;
			g_sDrvPWMBHandler.pfnPWM0CallBack = pfncallback;
			break;
		case DRVPWM_CAP4:			
			if (Rflag)
				PWMB->CCR0.CRL_IE0 = 1;
			if (Fflag)
				PWMB->CCR0.CFL_IE0 = 1;
		    g_sDrvPWMBHandler.pfnCAP0CallBack = pfncallback;
			break;	
		case DRVPWM_TIMER5:
			PWMB->PIER.PWMIE1 = 1;
			g_sDrvPWMBHandler.pfnPWM1CallBack = pfncallback;
			break;
		case DRVPWM_CAP5:			
		    if (Rflag)
				PWMB->CCR0.CRL_IE1 = 1;
			if (Fflag)
				PWMB->CCR0.CFL_IE1 = 1;
			g_sDrvPWMBHandler.pfnCAP1CallBack = pfncallback;
			break;
		case DRVPWM_TIMER6:
			PWMB->PIER.PWMIE2 = 1;
			g_sDrvPWMBHandler.pfnPWM2CallBack = pfncallback;
			break;
		case DRVPWM_CAP6:			
		    if (Rflag)
				PWMB->CCR1.CRL_IE2 = 1;
			if (Fflag)
				PWMB->CCR1.CFL_IE2 = 1;
			g_sDrvPWMBHandler.pfnCAP2CallBack = pfncallback;
			break;
		case DRVPWM_TIMER7:
			PWMB->PIER.PWMIE3 = 1;
			g_sDrvPWMBHandler.pfnPWM3CallBack = pfncallback;
			break;
		case DRVPWM_CAP7:			
			if (Rflag)
				PWMB->CCR1.CRL_IE3 = 1;
			if (Fflag)
				PWMB->CCR1.CFL_IE3 = 1;
		    g_sDrvPWMBHandler.pfnCAP3CallBack = pfncallback;
			break;				
	}							
		
	if (u8Timer & 0x04)
		NVIC_EnableIRQ(PWMB_IRQn);
	else
		NVIC_EnableIRQ(PWMA_IRQn);			 
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_DisableInt		                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*              							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   / DRVPWM_CAP2   / DRVPWM_CAP3    */
/*         									DRVPWM_CAP4   / DRVPWM_CAP5   / DRVPWM_CAP6   / DRVPWM_CAP7    */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None                                                                            		   */
/*               			                                                                          	   */
/* Description:                                                                                            */
/*               This function is used to clear the PWM0~7 timer/capture interrupt 			               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_DisableInt(uint8_t u8Timer)
{   	
	switch(u8Timer)
	{	
		case DRVPWM_TIMER0:
			PWMA->PIER.PWMIE0 = 0;
			PWMA->PIIR.PWMIF0 = 1;
			g_sDrvPWMAHandler.pfnPWM0CallBack = NULL;
			break;
		case DRVPWM_TIMER1:
			PWMA->PIER.PWMIE1 = 0;
			PWMA->PIIR.PWMIF1 = 1;
			g_sDrvPWMAHandler.pfnPWM1CallBack = NULL;
			break;			
		case DRVPWM_TIMER2:
			PWMA->PIER.PWMIE2 = 0;
			PWMA->PIIR.PWMIF2 = 1;
			g_sDrvPWMAHandler.pfnPWM2CallBack = NULL;
			break;		
		case DRVPWM_TIMER3:
			PWMA->PIER.PWMIE3 = 0;
			PWMA->PIIR.PWMIF3 = 1;
			g_sDrvPWMAHandler.pfnPWM3CallBack = NULL;
			break;
		case DRVPWM_TIMER4:
			PWMB->PIER.PWMIE0 = 0;
			PWMB->PIIR.PWMIF0 = 1;
			g_sDrvPWMBHandler.pfnPWM0CallBack = NULL;
			break;
		case DRVPWM_TIMER5:
			PWMB->PIER.PWMIE1 = 0;
			PWMB->PIIR.PWMIF1 = 1;
			g_sDrvPWMBHandler.pfnPWM1CallBack = NULL;
			break;			
		case DRVPWM_TIMER6:
			PWMB->PIER.PWMIE2 = 0;
			PWMB->PIIR.PWMIF2 = 1;
			g_sDrvPWMBHandler.pfnPWM2CallBack = NULL;
			break;		
		case DRVPWM_TIMER7:
			PWMB->PIER.PWMIE3 = 0;
			PWMB->PIIR.PWMIF3 = 1;
			g_sDrvPWMBHandler.pfnPWM3CallBack = NULL;
			break;
		case DRVPWM_CAP0:			 		
			PWMA->CCR0.CRL_IE0 = 0;
			PWMA->CCR0.CFL_IE0 = 0;
			g_sDrvPWMAHandler.pfnCAP0CallBack = NULL;	
			break;				
		case DRVPWM_CAP1:
			PWMA->CCR0.CRL_IE1 = 0;
			PWMA->CCR0.CFL_IE1 = 0;
			g_sDrvPWMAHandler.pfnCAP1CallBack = NULL;	
			break;		
		case DRVPWM_CAP2:
			PWMA->CCR1.CRL_IE2 = 0;
			PWMA->CCR1.CFL_IE2 = 0;
			g_sDrvPWMAHandler.pfnCAP2CallBack = NULL;	
			break;
		case DRVPWM_CAP3:
			PWMA->CCR1.CRL_IE3 = 0;
			PWMA->CCR1.CFL_IE3 = 0;
			g_sDrvPWMAHandler.pfnCAP3CallBack = NULL;	
			break;       	    							
		case DRVPWM_CAP4:			 		
			PWMB->CCR0.CRL_IE0 = 0;
			PWMB->CCR0.CFL_IE0 = 0;
			g_sDrvPWMBHandler.pfnCAP0CallBack = NULL;	
			break;				
		case DRVPWM_CAP5:
			PWMB->CCR0.CRL_IE1 = 0;
			PWMB->CCR0.CFL_IE1 = 0;
			g_sDrvPWMBHandler.pfnCAP1CallBack = NULL;	
			break;		
		case DRVPWM_CAP6:
			PWMB->CCR1.CRL_IE2 = 0;
			PWMB->CCR1.CFL_IE2 = 0;
			g_sDrvPWMBHandler.pfnCAP2CallBack = NULL;	
			break;
		case DRVPWM_CAP7:
			PWMB->CCR1.CRL_IE3 = 0;
			PWMB->CCR1.CFL_IE3 = 0;
			g_sDrvPWMBHandler.pfnCAP3CallBack = NULL;	
			break; 
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_ClearInt		                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*              							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   / DRVPWM_CAP2   / DRVPWM_CAP3    */
/*         									DRVPWM_CAP4   / DRVPWM_CAP5   / DRVPWM_CAP6   / DRVPWM_CAP7    */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None                                                                            		   */
/*               			                                                                          	   */
/* Description:                                                                                            */
/*               This function is used to clear the PWM0~7 timer/capture interrupt 			               */
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_ClearInt(uint8_t u8Timer)
{
	if (u8Timer & 0x10)
	{
		switch (u8Timer)
		{
			case DRVPWM_CAP0:
				PWMA->CCR0.CAPIF0 = 1;
				break;
			case DRVPWM_CAP1:
				PWMA->CCR0.CAPIF1 = 1;				
				break;
			case DRVPWM_CAP2:
				PWMA->CCR1.CAPIF2 = 1;
				break;																	  
			case DRVPWM_CAP3:
				PWMA->CCR1.CAPIF3 = 1;
				break;
			case DRVPWM_CAP4:
				PWMB->CCR0.CAPIF0 = 1;
				break;
			case DRVPWM_CAP5:
				PWMB->CCR0.CAPIF1 = 1;				
				break;
			case DRVPWM_CAP6:
				PWMB->CCR1.CAPIF2 = 1;
				break;																	  
			case DRVPWM_CAP7:
				PWMB->CCR1.CAPIF3 = 1;
				break;
		}			
	}	
	else
	{
		switch (u8Timer)
		{
			case DRVPWM_TIMER0:
				PWMA->PIIR.PWMIF0 = 1;
				break;
			case DRVPWM_TIMER1:
				PWMA->PIIR.PWMIF1 = 1;				
				break;
			case DRVPWM_TIMER2:
				PWMA->PIIR.PWMIF2 = 1;
				break;																	  
			case DRVPWM_TIMER3:
				PWMA->PIIR.PWMIF3 = 1;
				break;
			case DRVPWM_TIMER4:
				PWMB->PIIR.PWMIF0 = 1;
				break;
			case DRVPWM_TIMER5:
				PWMB->PIIR.PWMIF1 = 1;				
				break;
			case DRVPWM_TIMER6:
				PWMB->PIIR.PWMIF2 = 1;
				break;																	  
			case DRVPWM_TIMER7:
				PWMB->PIIR.PWMIF3 = 1;
				break;
		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetIntFlag				                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*              							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   / DRVPWM_CAP2   / DRVPWM_CAP3    */
/*         									DRVPWM_CAP4   / DRVPWM_CAP5   / DRVPWM_CAP6   / DRVPWM_CAP7    */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               0		FALSE                                                                              */
/*               1		TRUE		                                                                       */
/* Description:                                                                                            */
/*               This function is used to get the PWM0~7 timer/capture interrupt flag 			           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPWM_GetIntFlag(uint8_t u8Timer)
{
	int32_t status = 0;
	
	if (u8Timer & 0x10)
	{
		switch (u8Timer)
		{
			case DRVPWM_CAP0:
				status = PWMA->CCR0.CAPIF0;
				break;
			case DRVPWM_CAP1:
				status = PWMA->CCR0.CAPIF1;				
				break;
			case DRVPWM_CAP2:
				status = PWMA->CCR1.CAPIF2;
				break;																	  
			case DRVPWM_CAP3:
				status = PWMA->CCR1.CAPIF3;
				break;
			case DRVPWM_CAP4:
				status = PWMB->CCR0.CAPIF0;
				break;
			case DRVPWM_CAP5:
				status = PWMB->CCR0.CAPIF1;				
				break;
			case DRVPWM_CAP6:
				status = PWMB->CCR1.CAPIF2;
				break;																	  
			case DRVPWM_CAP7:
				status = PWMB->CCR1.CAPIF3;
		  		break;
		}		
	}
	else
	{
		switch (u8Timer)
		{
			case DRVPWM_TIMER0:
				status = PWMA->PIIR.PWMIF0;
				break;
			case DRVPWM_TIMER1:
				status = PWMA->PIIR.PWMIF1;				
				break;
			case DRVPWM_TIMER2:
				status = PWMA->PIIR.PWMIF2;
				break;																	  
			case DRVPWM_TIMER3:
				status = PWMA->PIIR.PWMIF3;
				break;
			case DRVPWM_TIMER4:
				status = PWMB->PIIR.PWMIF0;
				break;
			case DRVPWM_TIMER5:
				status = PWMB->PIIR.PWMIF1;				
				break;
			case DRVPWM_TIMER6:
				status = PWMB->PIIR.PWMIF2;
				break;																	  
			case DRVPWM_TIMER7:
				status = PWMB->PIIR.PWMIF3;
				break;
		}	
	}

	return status;		
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetRisingCounter				                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1 / DRVPWM_CAP2 / DRVPWM_CAP3	       */
/*               							DRVPWM_CAP4 / DRVPWM_CAP5 / DRVPWM_CAP6 / DRVPWM_CAP7	       */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The value which latches the counter when there・s a rising transition                     */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to get value which latches the counter when there・s a rising 	   */		
/*				 transition		                   														   */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t DrvPWM_GetRisingCounter(uint8_t u8Capture)
{
	uint32_t u32Reg = 0;
	
	if (u8Capture >= DRVPWM_CAP4)
	{
		u32Reg = *((__IO uint32_t *) (PWMB_CRLR0 + ((u8Capture - DRVPWM_CAP4) << 3)));
	}
	else
	{
		u32Reg = *((__IO uint32_t *) (PWMA_CRLR0 + ((u8Capture & 0x7) << 3)));	
	}

	return u32Reg;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetFallingCounter				                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1 / DRVPWM_CAP2 / DRVPWM_CAP3	       */
/*               							DRVPWM_CAP4 / DRVPWM_CAP5 / DRVPWM_CAP6 / DRVPWM_CAP7	       */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The value which latches the counter when there・s a falling transition                    */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to get value which latches the counter when there・s a falling 	   */		
/*				 transition		                   														   */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t DrvPWM_GetFallingCounter(uint8_t u8Capture)
{
	uint32_t u32Reg = 0;
	
	if (u8Capture >= DRVPWM_CAP4)
	{
		u32Reg = *((__IO uint32_t *) (PWMB_CFLR0 + ((u8Capture - DRVPWM_CAP4) << 3)));
	}
	else
	{
		u32Reg = *((__IO uint32_t *) (PWMA_CFLR0 + ((u8Capture & 0x7) << 3)));	
	}

	return u32Reg;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_GetCaptureIntStatus				                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1 / DRVPWM_CAP2 / DRVPWM_CAP3	       */
/*               							DRVPWM_CAP4 / DRVPWM_CAP5 / DRVPWM_CAP6 / DRVPWM_CAP7	       */
/*               u8IntType 		- [in]		DRVPWM_CAP_RISING_FLAG/DRVPWM_CAP_FALLING_FLAG	       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               Check if there・s a rising / falling transition                    				       */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to get the rising / falling transition interrupt flag			   */		
/*---------------------------------------------------------------------------------------------------------*/
int32_t DrvPWM_GetCaptureIntStatus(uint8_t u8Capture, uint8_t u8IntType)
{	
	int32_t status;

	status = 0;	

	switch (u8Capture)
	{
		case DRVPWM_CAP0:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMA->CCR0.CRLRI0 : PWMA->CCR0.CFLRI0;
			break;
		case DRVPWM_CAP1:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMA->CCR0.CRLRI1 : PWMA->CCR0.CFLRI1;
			break;
		case DRVPWM_CAP2:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMA->CCR1.CRLRI2 : PWMA->CCR1.CFLRI2;
			break;																	  
		case DRVPWM_CAP3:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMA->CCR1.CRLRI3 : PWMA->CCR1.CFLRI3;
			break;
		case DRVPWM_CAP4:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMB->CCR0.CRLRI0 : PWMB->CCR0.CFLRI0;
			break;
		case DRVPWM_CAP5:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMB->CCR0.CRLRI1 : PWMB->CCR0.CFLRI1;
			break;
		case DRVPWM_CAP6:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMB->CCR1.CRLRI2 : PWMB->CCR1.CFLRI2;
			break;																	  
		case DRVPWM_CAP7:
			status = (u8IntType == DRVPWM_CAP_RISING_FLAG)? PWMB->CCR1.CRLRI3 : PWMB->CCR1.CFLRI3;
			break;
	}
	
	return status;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_ClearCaptureIntStatus				                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Capture 		- [in]		DRVPWM_CAP0 / DRVPWM_CAP1 / DRVPWM_CAP2 / DRVPWM_CAP3	       */
/*               							DRVPWM_CAP4 / DRVPWM_CAP5 / DRVPWM_CAP6 / DRVPWM_CAP7	       */
/*               u8IntType 		- [in]		DRVPWM_CAP_RISING_FLAG/DRVPWM_CAP_FALLING_FLAG	       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               Clear the rising / falling transition interrupt flag	                    		       */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to clear the rising / falling transition interrupt flag		 	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_ClearCaptureIntStatus(uint8_t u8Capture, uint8_t u8IntType)
{
	switch (u8Capture)
	{
		case DRVPWM_CAP0:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMA->CCR0.CRLRI0 = 0) : (PWMA->CCR0.CFLRI0 = 0);
			break;
		case DRVPWM_CAP1:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMA->CCR0.CRLRI1 = 0) : (PWMA->CCR0.CFLRI1 = 0);
			break;
		case DRVPWM_CAP2:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMA->CCR1.CRLRI2 = 0) : (PWMA->CCR1.CFLRI2 = 0);
			break;																	  
		case DRVPWM_CAP3:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMA->CCR1.CRLRI3 = 0) : (PWMA->CCR1.CFLRI3 = 0);
			break;
		case DRVPWM_CAP4:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMB->CCR0.CRLRI0 = 0) : (PWMB->CCR0.CFLRI0 = 0);
			break;
		case DRVPWM_CAP5:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMB->CCR0.CRLRI1 = 0) : (PWMB->CCR0.CFLRI1 = 0);
			break;
		case DRVPWM_CAP6:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMB->CCR1.CRLRI2 = 0) : (PWMB->CCR1.CFLRI2 = 0);
			break;																	  
		case DRVPWM_CAP7:
			(u8IntType == DRVPWM_CAP_RISING_FLAG)? (PWMB->CCR1.CRLRI3 = 0) : (PWMB->CCR1.CFLRI3 = 0);
			break;
	} 
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_Open				                                                   				   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None             											                               */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None 											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Enable PWM engine clock and reset PWM											 	   	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_Open(void)
{ 
	SYSCLK->APBCLK.PWM01_EN = 1;
	SYSCLK->APBCLK.PWM23_EN = 1;
	SYSCLK->APBCLK.PWM45_EN = 1;
	SYSCLK->APBCLK.PWM67_EN = 1;
	SYS->IPRSTC2.PWM03_RST = 1;
	SYS->IPRSTC2.PWM03_RST = 0;
	SYS->IPRSTC2.PWM47_RST = 1;
	SYS->IPRSTC2.PWM47_RST = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_Close				                                                   				   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               None             											                               */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None 											                    		       		   */
/*               							                                                               */
/* Description:                                                                                            */
/*               Disable PWM engine clock and the I/O enable											   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_Close(void)
{
	PWMA->POE.PWM0 = 0;
	PWMA->POE.PWM1 = 0;
	PWMA->POE.PWM2 = 0;
	PWMA->POE.PWM3 = 0;
	PWMA->CAPENR = 0;

	PWMB->POE.PWM0 = 0;
	PWMB->POE.PWM1 = 0;
	PWMB->POE.PWM2 = 0;
	PWMB->POE.PWM3 = 0;
	PWMB->CAPENR = 0;

	NVIC_DisableIRQ(PWMA_IRQn);
	NVIC_DisableIRQ(PWMB_IRQn);
	
	SYS->IPRSTC2.PWM03_RST = 1;
	SYS->IPRSTC2.PWM03_RST = 0;
	SYS->IPRSTC2.PWM47_RST = 1;
	SYS->IPRSTC2.PWM47_RST = 0;

	SYSCLK->APBCLK.PWM01_EN = 0;
	SYSCLK->APBCLK.PWM23_EN = 0;
	SYSCLK->APBCLK.PWM45_EN = 0;
	SYSCLK->APBCLK.PWM67_EN = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_EnableDeadZone				                                             		       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		   - [in]	DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*               							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*               u8Length 		   - [in]	Dead Zone Length : 0~255				       		   		   */
/*               i32EnableDeadZone - [in]	Enable DeadZone (1) / Diasble DeadZone (0)     		           */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to set the dead zone length and enable/disable Dead Zone function   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_EnableDeadZone(uint8_t u8Timer, uint8_t u8Length, int32_t i32EnableDeadZone)
{
	switch (u8Timer & 0x07)
	{
		case DRVPWM_TIMER0:
		case DRVPWM_TIMER1:
			PWMA->PPR.DZI01  = u8Length;
			PWMA->PCR.DZEN01 = i32EnableDeadZone;
			break;
		case DRVPWM_TIMER2:
		case DRVPWM_TIMER3:
			PWMA->PPR.DZI23  = u8Length;
			PWMA->PCR.DZEN23 = i32EnableDeadZone;
			break;
		case DRVPWM_TIMER4:
		case DRVPWM_TIMER5:
			PWMB->PPR.DZI01  = u8Length;
			PWMB->PCR.DZEN01 = i32EnableDeadZone;
			break;
		case DRVPWM_TIMER6:
		case DRVPWM_TIMER7:
			PWMB->PPR.DZI23  = u8Length;
			PWMB->PCR.DZEN23 = i32EnableDeadZone;
			break;
	} 	
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_Enable				                            		                 		       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*               							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   / DRVPWM_CAP2   / DRVPWM_CAP3	   */
/*         									DRVPWM_CAP4   / DRVPWM_CAP5   / DRVPWM_CAP6   / DRVPWM_CAP7	   */
/*               i32Enable		- [in]		Enable  (1) / Disable  (0)     					       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to enable PWM0~7 timer/capture function						 	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_Enable(uint8_t u8Timer, int32_t i32Enable)
{
	switch (u8Timer & 0x07)
	{
		case DRVPWM_TIMER0:
			PWMA->PCR.CH0EN = i32Enable;
			break;
		case DRVPWM_TIMER1:
			PWMA->PCR.CH1EN = i32Enable;
			break;
		case DRVPWM_TIMER2:
			PWMA->PCR.CH2EN = i32Enable;
			break;
		case DRVPWM_TIMER3:
			PWMA->PCR.CH3EN = i32Enable;
			break;
		case DRVPWM_TIMER4:
			PWMB->PCR.CH0EN = i32Enable;
			break;
		case DRVPWM_TIMER5:
			PWMB->PCR.CH1EN = i32Enable;
			break;
		case DRVPWM_TIMER6:
			PWMB->PCR.CH2EN = i32Enable;
			break;
		case DRVPWM_TIMER7:
			PWMB->PCR.CH3EN = i32Enable;
			break;
	} 	

	if (u8Timer & 0x10)
	{
		switch (u8Timer)
		{
			case DRVPWM_CAP0:
				PWMA->CCR0.CAPCH0EN = i32Enable;
				break;
			case DRVPWM_CAP1:
				PWMA->CCR0.CAPCH1EN = i32Enable;
				break;
			case DRVPWM_CAP2:
				PWMA->CCR1.CAPCH2EN = i32Enable;	
				break;																	  
			case DRVPWM_CAP3:
				PWMA->CCR1.CAPCH3EN = i32Enable;				
				break;
			case DRVPWM_CAP4:
				PWMB->CCR0.CAPCH0EN = i32Enable;
				break;
			case DRVPWM_CAP5:
				PWMB->CCR0.CAPCH1EN = i32Enable;
				break;
			case DRVPWM_CAP6:
				PWMB->CCR1.CAPCH2EN = i32Enable;	
				break;																	  
			case DRVPWM_CAP7:
				PWMB->CCR1.CAPCH3EN = i32Enable;				
				break;	
		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SetTimerClk					                                             		       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 						DRVPWM_TIMER0/DRVPWM_TIMER1/DRVPWM_TIMER2/DRVPWM_TIMER3    */
/*               								DRVPWM_TIMER4/DRVPWM_TIMER5/DRVPWM_TIMER6/DRVPWM_TIMER7    */
/*         										DRVPWM_CAP0/DRVPWM_CAP1/DRVPWM_CAP2/DRVPWM_CAP3        	   */
/*         										DRVPWM_CAP4/DRVPWM_CAP5/DRVPWM_CAP6/DRVPWM_CAP7        	   */
/*				 S_DRVPWM_TIME_DATA_T *sPt																   */
/*               u8Frequency					Frequency   					       					   */
/*               u8HighPulseRatio				High Pulse Ratio    					       			   */
/*               u8Mode							DRVPWM_ONE_SHOT_MODE / DRVPWM_TOGGLE_MODE 				   */
/*               bInverter						Inverter Enable  (TRUE) / Inverter Disable  (FALSE)        */
/*               u8ClockSelector				Clock Selector											   */
/*              								DRVPWM_CLOCK_DIV_1/DRVPWM_CLOCK_DIV_2/DRVPWM_CLOCK_DIV_4   */
/*              								DRVPWM_CLOCK_DIV_8/DRVPWM_CLOCK_DIV_16					   */
/*												(The parameter takes effect when u8Frequency = 0)		   */
/*               u8PreScale						Prescale (2 ~ 256)										   */ 
/*												(The parameter takes effect when u8Frequency = 0)		   */
/*               u32Duty						Pulse duty										           */                             
/*												(The parameter takes effect when u8Frequency = 0 or		   */
/*												u8Timer = DRVPWM_CAP0/DRVPWM_CAP1/DRVPWM_CAP2/DRVPWM_CAP3) */	
/*              											                                        	   */
/* Returns:                                                                                                */
/*               The actual frequency											                           */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to configure the frequency/pulse/mode/inverter function		       */		
/*---------------------------------------------------------------------------------------------------------*/
uint32_t DrvPWM_SetTimerClk(uint8_t u8Timer, S_DRVPWM_TIME_DATA_T *sPt)
{
	uint32_t 	u32Frequency;
	uint32_t	u32Freqout;
	uint16_t	u16Duty, u16cnrData, u16cmrData;
	uint8_t	    u8EngineClk = 0;
		
	switch (u8Timer & 0x07)
	{
		case DRVPWM_TIMER0:
		case DRVPWM_TIMER1:
			u8EngineClk = SYSCLK->CLKSEL1.PWM01_S;				
			break;
		case DRVPWM_TIMER2:
		case DRVPWM_TIMER3:
			u8EngineClk = SYSCLK->CLKSEL1.PWM23_S;
			break;		
		case DRVPWM_TIMER4:
		case DRVPWM_TIMER5:
			u8EngineClk = SYSCLK->CLKSEL2.PWM45_S;				
			break;
		case DRVPWM_TIMER6:
		case DRVPWM_TIMER7:
			u8EngineClk = SYSCLK->CLKSEL2.PWM67_S;
			break;	
	}


	if (u8EngineClk == 0)		 /* external 12MHz crystal clock	*/
	{
		u32Freqout = 12000000;	
	}
	else if(u8EngineClk == 1)    /* external 32KHz crystal clock */
	{
		u32Freqout = 32000;
	}
	else if(u8EngineClk == 2)	 /* HCLK clock */
	{
		u32Freqout = DrvSYS_GetHCLK()*1000;
	}
	else						 /* internal 22MHz oscillator clock */
	{
	 	u32Freqout = 22000000;	
	}

	switch (u8Timer)
	{
		case DRVPWM_TIMER0:
			PWMA->PCR.CH0INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER1:
			PWMA->PCR.CH1INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER2:
			PWMA->PCR.CH2INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER3:
			PWMA->PCR.CH3INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER4:
			PWMB->PCR.CH0INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER5:
			PWMB->PCR.CH1INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER6:
			PWMB->PCR.CH2INV = sPt->i32Inverter;
			break;
		case DRVPWM_TIMER7:
			PWMB->PCR.CH3INV = sPt->i32Inverter;
			break;

		case DRVPWM_CAP0:
			PWMA->CCR0.INV0 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP1:
			PWMA->CCR0.INV1 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP2:
			PWMA->CCR1.INV2 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP3:
			PWMA->CCR1.INV3 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP4:
			PWMB->CCR0.INV0 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP5:
			PWMB->CCR0.INV1 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP6:
			PWMB->CCR1.INV2 = sPt->i32Inverter;
			break;
		case DRVPWM_CAP7:
			PWMB->CCR1.INV3 = sPt->i32Inverter;
			break;
	} 

	switch (u8Timer & 0x07)
	{
		case DRVPWM_TIMER0:
			PWMA->PCR.CH0MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER1:
			PWMA->PCR.CH1MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER2:
			PWMA->PCR.CH2MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER3:
			PWMA->PCR.CH3MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER4:
			PWMB->PCR.CH0MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER5:
			PWMB->PCR.CH1MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER6:
			PWMB->PCR.CH2MOD = sPt->u8Mode;
			break;
		case DRVPWM_TIMER7:
			PWMB->PCR.CH3MOD = sPt->u8Mode;
			break;
	} 
			
	
	if (sPt->u32Frequency == 0)
	{	
		uint8_t	u8Divider = 1;
		uint32_t u32duty;
		
		switch (u8Timer & 0x07)
		{
			case DRVPWM_TIMER0:
			case DRVPWM_TIMER1:
				PWMA->PPR.CP01 = sPt->u8PreScale - 1;
				break;
			case DRVPWM_TIMER2:
			case DRVPWM_TIMER3:
				PWMA->PPR.CP23 = sPt->u8PreScale - 1;
				break;
			case DRVPWM_TIMER4:
			case DRVPWM_TIMER5:
				PWMB->PPR.CP01 = sPt->u8PreScale - 1;
				break;
			case DRVPWM_TIMER6:
			case DRVPWM_TIMER7:
				PWMB->PPR.CP23 = sPt->u8PreScale - 1;
				break;
		} 
		
		u32duty = sPt->u32Duty * sPt->u8HighPulseRatio / 100 - 1;
		
		switch (u8Timer & 0x07)
		{
			case DRVPWM_TIMER0:
				PWMA->CSR.CSR0 = sPt->u8ClockSelector;
				PWMA->CNR0 = sPt->u32Duty - 1;
				PWMA->CMR0 = u32duty;
				break;
			case DRVPWM_TIMER1:
				PWMA->CSR.CSR1 = sPt->u8ClockSelector;
				PWMA->CNR1 = sPt->u32Duty - 1;
				PWMA->CMR1 = u32duty;
				break;
			case DRVPWM_TIMER2:
				PWMA->CSR.CSR2 = sPt->u8ClockSelector;
				PWMA->CNR2 = sPt->u32Duty - 1;
				PWMA->CMR2 = u32duty;
				break;
			case DRVPWM_TIMER3:
				PWMA->CSR.CSR3 = sPt->u8ClockSelector;
				PWMA->CNR3 = sPt->u32Duty - 1;
				PWMA->CMR3 = u32duty;
				break;
			case DRVPWM_TIMER4:
				PWMB->CSR.CSR0 = sPt->u8ClockSelector;
				PWMB->CNR0 = sPt->u32Duty - 1;
				PWMB->CMR0 = u32duty;
				break;
			case DRVPWM_TIMER5:
				PWMB->CSR.CSR1 = sPt->u8ClockSelector;
				PWMB->CNR1 = sPt->u32Duty - 1;
				PWMB->CMR1 = u32duty;
				break;
			case DRVPWM_TIMER6:
				PWMB->CSR.CSR2 = sPt->u8ClockSelector;
				PWMB->CNR2 = sPt->u32Duty - 1;
				PWMB->CMR2 = u32duty;
				break;
			case DRVPWM_TIMER7:
				PWMB->CSR.CSR3 = sPt->u8ClockSelector;
				PWMB->CNR3 = sPt->u32Duty - 1;
				PWMB->CMR3 = u32duty;
				break;
		}
				
		switch (sPt->u8ClockSelector)
		{
			case DRVPWM_CLOCK_DIV_1:
				u8Divider = 1;
				break;
			case DRVPWM_CLOCK_DIV_2:
				u8Divider = 2;			
				break;			
			case DRVPWM_CLOCK_DIV_4:
				u8Divider = 4;			
				break;			
			case DRVPWM_CLOCK_DIV_8:
				u8Divider = 8;			
				break;			
			case DRVPWM_CLOCK_DIV_16: 
				u8Divider = 16;			
				break;		
		}		
		
		u32Frequency = u32Freqout / sPt->u8PreScale / u8Divider / sPt->u32Duty;
	}
	else
	{
		uint8_t	 u8Divider;
		uint16_t u16PreScale;
		
		u32Frequency =  u32Freqout / sPt->u32Frequency;		
		
		if (u32Frequency > 0x10000000)
			return 0;
			
		u8Divider = 1;			
			
		if (u32Frequency < 0x20000)
			u16PreScale = 2;	
		else
		{
			u16PreScale = u32Frequency / 65536;	
							
			if (u32Frequency / u16PreScale > 65536)
				u16PreScale++;
			
			if (u16PreScale > 256)
			{
				uint8_t u8i = 0;
				
				u16PreScale = 256;
				u32Frequency = u32Frequency / u16PreScale;
				
				u8Divider = u32Frequency / 65536;				
				
				if(u32Frequency / u8Divider > 65536)
					u8Divider++;				
				
				while(1)	
				{
					if((1 << u8i++) > u8Divider)
						break;
				}
				
				u8Divider = 1 << (u8i - 1);
				
				if (u8Divider > 16)
					return 0;	
					
				u32Frequency = u32Frequency * u16PreScale;						
			}		
					
		}
		u16Duty = (uint16_t )(u32Frequency/u16PreScale/u8Divider);
		
		u32Frequency = (u32Freqout / u16PreScale / u8Divider) / u16Duty;	
				
		switch (u8Timer & 0x07)
		{
			case DRVPWM_TIMER0:
			case DRVPWM_TIMER1:
				PWMA->PPR.CP01 = u16PreScale - 1;
				break;
			case DRVPWM_TIMER2:
			case DRVPWM_TIMER3:
				PWMA->PPR.CP23 = u16PreScale - 1;
				break;
			case DRVPWM_TIMER4:
			case DRVPWM_TIMER5:
				PWMB->PPR.CP01 = u16PreScale - 1;
				break;
			case DRVPWM_TIMER6:
			case DRVPWM_TIMER7:
				PWMB->PPR.CP23 = u16PreScale - 1;
				break;
		}
		
		switch(u8Divider)
		{
			case 1:
				u8Divider = DRVPWM_CLOCK_DIV_1;
				break;
			case 2:
				u8Divider = DRVPWM_CLOCK_DIV_2;			
				break;			
			case 4:
				u8Divider = DRVPWM_CLOCK_DIV_4;			
				break;			
			case 8:
				u8Divider = DRVPWM_CLOCK_DIV_8;			
				break;			
			case 16:
				u8Divider = DRVPWM_CLOCK_DIV_16;			
				break;		
		}				
					
		if (u8Timer & 0x10)
		{
			g_u16cnrData = u16cnrData = sPt->u32Duty - 1; 
			u16cmrData = sPt->u32Duty * sPt->u8HighPulseRatio / 1000 - 1;		
		}
		else
		{
		 	g_u16cnrData = u16cnrData = u16Duty - 1;
			u16cmrData = u16Duty * sPt->u8HighPulseRatio / 100 - 1;
		}
		
		switch (u8Timer & 0x07)
		{
			case DRVPWM_TIMER0:
				PWMA->CSR.CSR0 = u8Divider;
				PWMA->CNR0 = u16cnrData;
				PWMA->CMR0 = u16cmrData;
				break;
			case DRVPWM_TIMER1:
				PWMA->CSR.CSR1 = u8Divider;
				PWMA->CNR1 = u16cnrData;
				PWMA->CMR1 = u16cmrData;
				break;
			case DRVPWM_TIMER2:
				PWMA->CSR.CSR2 = u8Divider;
				PWMA->CNR2 = u16cnrData;
				PWMA->CMR2 = u16cmrData;
				break;
			case DRVPWM_TIMER3:
				PWMA->CSR.CSR3 = u8Divider;
				PWMA->CNR3 = u16cnrData;
				PWMA->CMR3 = u16cmrData;
				break;
			case DRVPWM_TIMER4:
				PWMB->CSR.CSR0 = u8Divider;
				PWMB->CNR0 = u16cnrData;
				PWMB->CMR0 = u16cmrData;
				break;
			case DRVPWM_TIMER5:
				PWMB->CSR.CSR1 = u8Divider;
				PWMB->CNR1 = u16cnrData;
				PWMB->CMR1 = u16cmrData;
				break;
			case DRVPWM_TIMER6:
				PWMB->CSR.CSR2 = u8Divider;
				PWMB->CNR2 = u16cnrData;
				PWMB->CMR2 = u16cmrData;
				break;
			case DRVPWM_TIMER7:
				PWMB->CSR.CSR3 = u8Divider;
				PWMB->CNR3 = u16cnrData;
				PWMB->CMR3 = u16cmrData;
				break;
		}
	}
	
	return u32Frequency;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SetTimerIO				                                             		           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               u8Timer 		- [in]		DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3  */
/*               							DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7  */
/*         									DRVPWM_CAP0   / DRVPWM_CAP1   / DRVPWM_CAP2   / DRVPWM_CAP3	   */
/*         									DRVPWM_CAP4   / DRVPWM_CAP5   / DRVPWM_CAP6   / DRVPWM_CAP7	   */
/*               i32Enable		- [in]		Enable  (1) / Diasble  (0)     					       		   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to enable/disable PWM0~7 timer/capture I/O function				   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_SetTimerIO(uint8_t u8Timer, int32_t i32Enable)
{
	if (i32Enable)
	{
		if (u8Timer & 0x10)
		{
			if (u8Timer & 0x04)
			 	PWMB->CAPENR = PWMB->CAPENR | (1 << (u8Timer - DRVPWM_CAP4));
			else
				PWMA->CAPENR = PWMA->CAPENR | (1 << (u8Timer & 0x07));			
		}
		else
		{
			switch (u8Timer)
			{
				case DRVPWM_TIMER0:
					PWMA->POE.PWM0 = 1;
					break;
				case DRVPWM_TIMER1:
					PWMA->POE.PWM1 = 1;				
					break;
				case DRVPWM_TIMER2:
					PWMA->POE.PWM2 = 1;
					break;																	  
				case DRVPWM_TIMER3:
					PWMA->POE.PWM3 = 1;
					break;
				case DRVPWM_TIMER4:
					PWMB->POE.PWM0 = 1;
					break;
				case DRVPWM_TIMER5:
					PWMB->POE.PWM1 = 1;				
					break;
				case DRVPWM_TIMER6:
					PWMB->POE.PWM2 = 1;
					break;																	  
				case DRVPWM_TIMER7:
					PWMB->POE.PWM3 = 1;
					break;
			}		
		}
	}
	else
	{
		if (u8Timer & 0x10)			
		{
			if (u8Timer & 0x04)
			 	PWMB->CAPENR = PWMB->CAPENR & ~(1 << (u8Timer & 0x07));
			else
				PWMA->CAPENR = PWMA->CAPENR & ~(1 << (u8Timer & 0x07));			
		}
		else
		{
			switch (u8Timer)
			{
				case DRVPWM_TIMER0:
					PWMA->POE.PWM0 = 0;
					break;
				case DRVPWM_TIMER1:
					PWMA->POE.PWM1 = 0;				
					break;
				case DRVPWM_TIMER2:
					PWMA->POE.PWM2 = 0;
					break;																	  
				case DRVPWM_TIMER3:
					PWMA->POE.PWM3 = 0;
					break;
				case DRVPWM_TIMER4:
					PWMB->POE.PWM0 = 0;
					break;
				case DRVPWM_TIMER5:
					PWMB->POE.PWM1 = 0;				
					break;
				case DRVPWM_TIMER6:
					PWMB->POE.PWM2 = 0;
					break;																	  
				case DRVPWM_TIMER7:
					PWMB->POE.PWM3 = 0;
					break;
			}	
		}
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: DrvPWM_SelectClockSource				                                       		           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*           u8Timer 		        - [in] DRVPWM_TIMER0 / DRVPWM_TIMER1 / DRVPWM_TIMER2 / DRVPWM_TIMER3   */
/*           							   DRVPWM_TIMER4 / DRVPWM_TIMER5 / DRVPWM_TIMER6 / DRVPWM_TIMER7   */
/*           u8ClockSourceSelector  - [in] DRVPWM_EXT_12M/DRVPWM_EXT_32K/DRVPWM_HCLK/DRVPWM_INTERNAL_22M   */
/*              											                                        	   */
/* Returns:                                                                                                */
/*               None											                    		               */
/*               							                                                               */
/* Description:                                                                                            */
/*               This function is used to select PWM clock source                                   	   */		
/*---------------------------------------------------------------------------------------------------------*/
void DrvPWM_SelectClockSource(uint8_t u8Timer, uint8_t u8ClockSourceSelector)
{
	switch (u8Timer & 0x07)
	{
		case DRVPWM_TIMER0:
		case DRVPWM_TIMER1:
			SYSCLK->CLKSEL1.PWM01_S = u8ClockSourceSelector;				
			break;
		case DRVPWM_TIMER2:
		case DRVPWM_TIMER3:
			SYSCLK->CLKSEL1.PWM23_S = u8ClockSourceSelector;
			break;		
		case DRVPWM_TIMER4:
		case DRVPWM_TIMER5:
			SYSCLK->CLKSEL2.PWM45_S = u8ClockSourceSelector;				
			break;
		case DRVPWM_TIMER6:
		case DRVPWM_TIMER7:
			SYSCLK->CLKSEL2.PWM67_S = u8ClockSourceSelector;
			break;	
	}
}


