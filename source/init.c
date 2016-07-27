#include "MN-913A.h"

extern void HID_GetInReport( uint8_t *buf );
extern void HID_SetOutReport( uint8_t *pu8EpBuf, uint32_t u32Size );
uint16_t g_DDC114_CONV_Freq = 16, g_XenonFrequency = 32;
double Pass1_A260_Conc = 0.0, Start_Dvalid_Xenon_PWM_CNR1= 0.0, Stop_Dvalid_Xenon_PWM_CNR1= 0.0, Start_Dvalid_Xenon_PWM_CNR2 = 0.0, Stop_Dvalid_Xenon_PWM_CNR2 = 0.0;

void MN913A_init ( void ) {
  int i = 0, j = 0;
  STR_UART_T param;
	UART_T *tUART;
  /* Unlock the locked registers before we can access them */
  UNLOCKREG ( x );

  DrvSYS_SetOscCtrl( E_SYS_XTL12M, 1 );
  SYSCLK->PWRCON.XTL12M_EN = 1;
  /* Waiting for 12M Xtal stalble */
  SysTimerDelay(5000);
  //SYSCLK->PLLCON.FB_DV = 34;//62;
  SYSCLK->PLLCON.OE = 0x0;
  SYSCLK->PLLCON.PD= 0x0;	 
  //DrvSYS_SetPLLPowerDown(0);
  SYSCLK->PLLCON.BP = 0x0;
  /* HCLK clock source. 0: external 12MHz; 4:internal 22MHz RC oscillator */
  DrvSYS_SetHCLKSource(2);
  DrvSYS_SetClockDivider(E_SYS_HCLK_DIV, 0); /* HCLK clock frequency = HCLK
    clock source / (HCLK_N + 1) */
  //SystemCoreClockUpdate();

  //DrvGPIO_InitFunction(FUNC_UART0);
  param.u32BaudRate = 115200;
  param.u8cDataBits = DRVUART_DATABITS_8;
  param.u8cStopBits = DRVUART_STOPBITS_1;
  param.u8cParity = DRVUART_PARITY_NONE;
  param.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;
  param.u8TimeOut = 0;
  //DrvUART_Open(UART_PORT0, &param);

#ifdef USE_STN  //STN LCD
  DrvGPIO_InitFunction(E_FUNC_UART0);
  DrvUART_Open(DEBUG_INFO_PORT, &param);
#else  //TFT LCD
#ifdef MaestroNano_Fit_Mode  //fitting process
  DrvGPIO_InitFunction(FUNC_UART0);
  DrvUART_Open(DEBUG_INFO_PORT, &param);
#else
  DrvGPIO_InitFunction(FUNC_UART2);
  DrvUART_Open(DEBUG_INFO_PORT, &param);
#endif
#endif

  DrvGPIO_InitFunction(FUNC_UART1);
  //param.u32BaudRate = 115200;
	param.u32BaudRate = 9600;
  param.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;
  DrvUART_Open(TFT_COMM_PORT, &param);
  //DrvUART_DisableInt(TFT_COMM_PORT_ID, DRVUART_RLSNT | DRVUART_THREINT | DRVUART_RDAINT);
  //DrvUART_EnableInt(TFT_COMM_PORT_ID, DRVUART_RLSNT | DRVUART_RDAINT, UART_INT_HANDLE);

#ifdef MaestroNano_Fit_Mode  //fitting process
#else

  DrvGPIO_InitFunction(FUNC_UART0);
  //param.u32BaudRate = 9600;
  param.u32BaudRate = 115200;
  //param.u8cRxTriggerLevel = DRVUART_FIFO_30BYTES;
  DrvUART_Open(PRINTER_PORT, &param);
  DrvUART_DisableInt(PRINTER_PORT,DRVUART_RLSNT | DRVUART_THREINT | DRVUART_RDAINT | DRVUART_TOUTINT);
  DrvUART_EnableInt(PRINTER_PORT, DRVUART_RLSNT | DRVUART_RDAINT | DRVUART_TOUTINT, UART_INT_HANDLE);
	tUART = (UART_T *)((uint32_t)UART0 + PRINTER_PORT_ID);  
//#if Active_Printer==InHouse_Printer
/*20130808 added by michael*/
/*Drive RTS to low(low active)*/
  DrvUART_SetRTS(UART_PORT0, 1);
	tUART->FCR.RTS_TRIG_LEVEL = DRVUART_FIFO_8BYTES;
/*20130808 added by michael*/
//initialize the thermal printer
//reset printer, "ESC @"=="1B 40"
  //buf2[0] = 0x1b; buf2[1] = 0x40;
  //DrvUART_Write(PRINTER_PORT, buf2, 2);
//#endif // Active_Printer
#endif

  /* using divide frequency clock output, Ref. man pg.155-156 */
/* alternative ADC clock for TI-sensor board */
//#ifdef CONFIG_DIV_Clkout
  SYS->GPBMFP.CPO0_CLKO = 1;
  SYS->ALTMFP.PB12_CLKO = 1;  /* pin mux */
  SYSCLK->FRQDIV.FDIV_EN = 1;
  SYSCLK->APBCLK.FDIV_EN = 1;
  SYSCLK->FRQDIV.FSEL = 0;
  SYSCLK->CLKSEL2.FRQDIV_S = 2;
//#endif
  LOCKREG();

#ifdef USE_USB
  DrvUSB_Open((void *)DrvUSB_DispatchEvent);
  HID_Open((void *)HID_GetInReport, (void *)HID_SetOutReport); 
	/* Disable USB-related interrupts */
	_DRVUSB_ENABLE_MISC_INT(0);
	/* Enable float-detection interrupt */
	_DRVUSB_ENABLE_FLD_INT();
    /* Enable USB relative interrupt */    
	_DRVUSB_ENABLE_MISC_INT(IEF_WAKEUP | IEF_WAKEUPEN | IEF_FLD | IEF_USB | IEF_BUS);
#endif
}

void Init_Interface_IO(void)
{
  uint32_t u32HCLK, u32data, i, *pData;
  int32_t i32Err = 0;

#ifdef RTC_Power_ADC
  DrvGPIO_Open(GPA, 10, IO_OUTPUT);
#else
  DrvGPIO_Open(GPA, 7, IO_OUTPUT);
#endif

  //DrvGPIO_Open(GPB, 9, IO_INPUT); //up_key
  SYS->GPBMFP.TM1_SS11 = 0;
  /*20130417 added by michael
  switch PB.14/INT0 to  INT0*/
  SYS->GPBMFP.INT1_SS31 	=1;
  SYS->ALTMFP.PB14_S31    =0;
  //DrvGPIO_Open(GPB, 14, IO_INPUT);  //down_key
  DrvGPIO_Open(GPB, 13, IO_INPUT);  //enter_key
  DrvGPIO_Open(GPC, 6, IO_INPUT);  //inform MCU--->PC0
	DrvGPIO_Open(GPC, 15, IO_INPUT);
  DrvGPIO_Open(GPE, 5, IO_OUTPUT);  //output to printer Power control
	DrvGPIO_ClrBit(GPE, 5);
  //DrvGPIO_EnableInt(GPB, 9, IO_FALLING, MODE_EDGE);
  //DrvGPIO_EnableInt(GPB, 14, IO_FALLING, MODE_EDGE);
  DrvGPIO_EnableInt(GPB, 13, IO_FALLING, MODE_EDGE);
  DrvGPIO_EnableInt(GPC, 6, IO_FALLING, MODE_EDGE);
	DrvGPIO_EnableInt(GPC, 15, IO_BOTH_EDGE, MODE_EDGE);
  DrvGPIO_SetDebounceTime(1, DBCLKSRC_10K);
  //DrvGPIO_EnableDebounce(GPB, 9);
  //DrvGPIO_EnableDebounce(GPB, 15);
  DrvGPIO_EnableDebounce(GPB, 14);
  DrvGPIO_EnableDebounce(GPB, 13);
  //DrvGPIO_EnableDebounce(GPC, 0);
  DrvGPIO_EnableDebounce(GPC, 6);
	DrvGPIO_EnableDebounce(GPC, 15);

  DrvGPIO_SetIntCallback(GPABCallback, GPCDECallback);
  DrvGPIO_EnableEINT0(IO_FALLING, MODE_EDGE, EINT0Callback);
  //DrvGPIO_EnableEINT1(IO_FALLING, MODE_EDGE, EINT1Callback);
  outpw((uint32_t)&GPIOE->DOUT, inpw((uint32_t)&GPIOE->PIN) | (1 << 5));

//enable for external Xenon&Conv
  DrvGPIO_Open(GPB, 10, IO_OUTPUT); //Conv
  DrvGPIO_SetBit(GPB, 10);
  //DrvGPIO_ClrBit(GPB, 3);
  DrvGPIO_Open(GPB, 11, IO_OUTPUT); //Xenon PWM
  DrvGPIO_ClrBit(GPB, 11);
  DrvGPIO_Open(GPB, 9, IO_OUTPUT);
	DrvGPIO_ClrBit( GPB, 9 );
/*20160421 added by michael*/
  DrvGPIO_Open(GPA, 14, IO_INPUT);
  DrvGPIO_EnableInt(GPA, 14, IO_FALLING, MODE_EDGE);
	DrvGPIO_EnableDebounce(GPA, 14);
	
	/*DrvGPIO_Open(GPC, 3, IO_OUTPUT);
	DrvGPIO_ClrBit( GPC, 3 );
	SysTimerDelay( 5000 );
	DrvGPIO_SetBit( GPC, 3 );*/
	DrvGPIO_Open(GPC, 14, IO_OUTPUT);
	DrvGPIO_SetBit(GPC, 14);



  DrvGPIO_InitFunction(FUNC_I2C0);  //I2C for accessing external RTC
  UNLOCKREG();
  u32HCLK = DrvSYS_GetHCLK() * 1000;
  DrvI2C_Open(I2C_PORT0, u32HCLK, 200000);
  /* Get I2C0 clock */
  u32data = DrvI2C_GetClock(I2C_PORT0, u32HCLK);
#ifdef SYS_DEBUG
  printf("I2C0 clock %d Hz\n", u32data);
#endif
  /* Enable I2C0 interrupt and set corresponding NVIC bit */
  DrvI2C_EnableInt(I2C_PORT0);
  DrvI2C_EnableTimeoutCount(I2C_PORT0, 0, 0);  //starting time-out
  //DrvI2C_InstallCallback(I2C_PORT0, TIMEOUT, I2C0_Timeout);
  LOCKREG();
}

void DDC114_Init(void) {
	// DDC114.DATA_VALID																										 	
  SYS->GPAMFP.PWM1 		= 0;
  DrvGPIO_Open(GPA, 13, IO_INPUT);
  DrvGPIO_EnableInt(GPA, 13, IO_FALLING, MODE_EDGE);

  // DDC114.DCLK, DDC114.DOUT
  DrvSPI_Open(eDRVSPI_PORT1, eDRVSPI_MASTER, eDRVSPI_TYPE1, 20);
  /* Enable the automatic slave select function of SS0. */
  DrvSPI_EnableAutoCS(eDRVSPI_PORT1, eDRVSPI_SS0);
  /* Set the active level of slave select. */
  DrvSPI_SetSlaveSelectActiveLevel(eDRVSPI_PORT1, eDRVSPI_ACTIVE_LOW_FALLING);
  /* Enable the automatic slave select function and set the specified slave select pin. */
  DrvSPI_EnableAutoCS(eDRVSPI_PORT1, eDRVSPI_SS0);
  /* SPI clock rate 3MHz */
  DrvSPI_SetClock(eDRVSPI_PORT1, SPI_Clock, 0);
  //DrvSPI_BurstTransfer(eDRVSPI_PORT0, 2, 2);	

  SYS->GPCMFP.SPI1_SS0 	=0;
  DrvGPIO_Open(GPC, 8, IO_OUTPUT);
  outpw((uint32_t) &GPIOC->DOUT, inpw((uint32_t) &GPIOC->PIN) & 0xfffffeff);
  outpw(&SYS->GPBMFP, inpw(&SYS->GPBMFP) & ~(0x1<<9));  //PB.9/SS11 reset to PB.9
}

void PWM_setting_timer(S_DRVPWM_TIME_DATA_T sPt, uint8_t u8Timer, PFN_DRVPWM_CALLBACK pfncallback) {

  /* PWM Timer property */
  uint32_t status;
  /* Select PWM engine clock */
  DrvPWM_SelectClockSource(u8Timer&0xef, DRVPWM_HCLK);

  /* Set PWM Timer0 Configuration */
  status = DrvPWM_SetTimerClk(u8Timer, &sPt);
#ifdef SYS_DEBUG
  printf("PWM clock u32Frequency: %d\n", status);
#endif
  /* Enable Output for PWM Timer0 */
  DrvPWM_SetTimerIO(u8Timer&0xef, 1);

  /* Enable Interrupt Sources of PWM Timer0 and install call back function */
  DrvPWM_EnableInt(u8Timer&0xef, 0, pfncallback);
  /* Enable the PWM Timer 0 */	
}

void PWM_Init(void) {
  S_DRVPWM_TIME_DATA_T sPt;
  uint8_t u8Timer;

  DrvPWM_Open();
  /* Set PWM pins */
  DrvGPIO_InitFunction(FUNC_PWM01);
  DrvGPIO_InitFunction(FUNC_PWM23);

//PWM0/PA.12 for DDC114.CONV
  sPt.u8Mode = DRVPWM_TOGGLE_MODE;
  sPt.u32Frequency = g_DDC114_CONV_Freq;

#if (PWM_Count_Method==PWM_Param3) || (PWM_Count_Method==PWM_Param4) || (PWM_Count_Method==PWM_Param5)
  sPt.u8HighPulseRatio = 50; /* High Pulse peroid : Total Pulse peroid = 1 : 100 */
#else  //PWM_Count_Method==PWM_Param2
  sPt.u8HighPulseRatio = 3; /* High Pulse peroid : Total Pulse peroid = 1 : 100 */
#endif

  sPt.i32Inverter = 0;
  u8Timer = DRVPWM_TIMER0;
  PWM_setting_timer(sPt, u8Timer, NULL);

//PWM3/PA.15 for Xenon flash
  sPt.u8Mode = DRVPWM_TOGGLE_MODE;
  sPt.u32Frequency = g_XenonFrequency;
  sPt.u8HighPulseRatio = 1; /* High Pulse peroid : Total Pulse peroid = 1 : 100 */
  sPt.i32Inverter = 0;
  sPt.u32Duty = (48000000/g_XenonFrequency)/65536;
  if ((48000000/g_XenonFrequency)/sPt.u32Duty > 65536)
    sPt.u32Duty++;
  sPt.u32Duty = ((48000000/g_XenonFrequency) / sPt.u32Duty);
  u8Timer = DRVPWM_TIMER3;
  PWM_setting_timer(sPt, u8Timer | 0x10, NULL);

//20121109 modified FW10
#if PWM_Count_Method==PWM_Param2
  Start_Dvalid_Xenon_PWM_CNR1 = 65217*(1875)/62500;
  Stop_Dvalid_Xenon_PWM_CNR1 = 65217*(1875-450)/62500;
  Start_Dvalid_Xenon_PWM_CNR2 = 65217;
  Stop_Dvalid_Xenon_PWM_CNR2 = 65217 - 65217*(600)/62500;
#else
  Start_Dvalid_Xenon_PWM_CNR1 = 65217;
  Stop_Dvalid_Xenon_PWM_CNR1 = 65217 - 65217*(600)/62500;
  Start_Dvalid_Xenon_PWM_CNR2 = 65217 - 65217*(31250)/62500;
  Stop_Dvalid_Xenon_PWM_CNR2 = 65217 - 65217*(600+31250)/62500;
#endif
}

void MaestroNano_Init(void) {
  /* Unlock the locked registers before we can access them */
  UNLOCKREG(x);
//  Console_Init();
  PWM_Init();
  DDC114_Init();
  LOCKREG();
}