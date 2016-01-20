#include <MN-913A.h>

volatile uint8_t bPrinter_status;

#ifdef PRINTER_PORT
#define RXBUFSIZE 64
volatile uint8_t comRbuf[RXBUFSIZE];
volatile uint16_t comRbytes = 0;		/* Available receiving bytes */
volatile uint16_t comRhead 	= 0;
volatile uint16_t comRtail 	= 0;
void UART_INT_HANDLE(uint32_t u32IntStatus)
{
	uint8_t bInChar[1]={0xFF};
	

	if(u32IntStatus & DRVUART_RDAINT || (u32IntStatus & DRVUART_TOUTINT))
	{
		//printf("\nInput:");
		
		/* Get all the input characters */
#if PRINTER_PORT_ID==0
		while(UART0->ISR.RDA_IF==1 || UART0->ISR.TOUT_IF==1)
#elif  PRINTER_PORT_ID==1
		while(UART1->ISR.RDA_IF==1 || UART0->ISR.TOUT_IF==1)
#else
#endif
		{
			/* Get the character from UART Buffer */
			DrvUART_Read(PRINTER_PORT,bInChar,1);
            bPrinter_status = bInChar[0];
			printf("\n\rRDA_IF printer status: %x\n\r", bInChar[0]);
			
			/*if(bInChar[0] == '0')	
			{	
				g_bWait = FALSE;
			}*/
		
			/* Check if buffer full */
			if(comRbytes < RXBUFSIZE)
			{
				/* Enqueue the character */
				comRbuf[comRtail] = bInChar[0];
				comRtail = (comRtail == (RXBUFSIZE-1)) ? 0 : (comRtail+1);
				comRbytes++;
			}			
		}
		//printf("\nTransmission Test:");
	}
	else if(u32IntStatus & DRVUART_THREINT)
	{   
		   
        uint16_t tmp;
        tmp = comRtail;
			//bInChar[0] = comRbuf[comRhead];
			//DrvUART_Write(UART_PORT1,bInChar,1);
//printf("\nTransmission Test1:");
		if(comRhead != tmp)
		{
			bInChar[0] = comRbuf[comRhead];
  		    if (bInChar[0] < 128 && bInChar[0] >= 32) {
//#ifdef MaestroNano_Fit_Mode
			  DrvUART_Write(DEBUG_INFO_PORT,bInChar,1);
//#endif
			}
			else
				printf("\n\r THREIE status: %x\n\r", bInChar[0]);
			comRhead = (comRhead == (RXBUFSIZE-1)) ? 0 : (comRhead+1);
			comRbytes--;
		}
	}
}
#endif

void GPCDECallback(uint32_t u32GpcStatus, uint32_t u32GpdStatus, uint32_t u32GpeStatus)
{
}

void EINT0Callback(void)
{
}

void GPABCallback(uint32_t u32GpaStatus, uint32_t u32GpbStatus)
{
  int i = 0;
  double cur_Xenon_PWM_CNR = 0;
  static int knight = 0;
#ifdef SYS_DEBUG
  printf("GPAB Interrupt! GPA:0x%04x  GPB:0x%04x\n", u32GpaStatus, u32GpbStatus);
#endif
  if ((u32GpaStatus & 0x2000) && (adc_data_ready < (Measure_times+6))) {
    cur_Xenon_PWM_CNR = DrvPWM_GetTimerCounter(DRVPWM_TIMER0) ;
	/*Data_Valid_CNR[knight++] = cur_Xenon_PWM_CNR;
	if (knight%100==0)
	  for (i = 0; i <  knight && i < 100; i++)
	     printf("index: %d, CNR: %lf\n", i, Data_Valid_CNR[i]);
      if (knight==100)
	    knight = 0;*/

	//printf("cur_Xenon_PWM_CNR: %lf, %lf, %lf, %lf, %lf %d\n", cur_Xenon_PWM_CNR, 	Start_Dvalid_Xenon_PWM_CNR1, Stop_Dvalid_Xenon_PWM_CNR1, Start_Dvalid_Xenon_PWM_CNR2, Stop_Dvalid_Xenon_PWM_CNR2, knight++);
	//printf("%d\n", count1);
    //if ((cur_Xenon_PWM_CNR < Start_Dvalid_Xenon_PWM_CNR1 && cur_Xenon_PWM_CNR > Stop_Dvalid_Xenon_PWM_CNR1) || (cur_Xenon_PWM_CNR < Start_Dvalid_Xenon_PWM_CNR2 && cur_Xenon_PWM_CNR > Stop_Dvalid_Xenon_PWM_CNR2)) {
	if (!cur_phase || (cur_phase==1 &&(cur_Xenon_PWM_CNR < Start_Dvalid_Xenon_PWM_CNR1 && cur_Xenon_PWM_CNR > Stop_Dvalid_Xenon_PWM_CNR1) || (cur_Xenon_PWM_CNR < Start_Dvalid_Xenon_PWM_CNR2 && cur_Xenon_PWM_CNR > Stop_Dvalid_Xenon_PWM_CNR2))) {
//    adc_data_ready++;
#ifdef DDC114_DEBUG
	outpw((uint32_t)&GPIOA->DOUT, inpw((uint32_t)&GPIOA->PIN) ^ (1 << 11));
#endif
    }
	else {
	   redundancy_entry++;
	   return;
	}

    if (adc_data_ready >= 6) {
      do
      {
        SPI1->CNTRL.GO_BUSY = 1;
        while (SPI1->CNTRL.GO_BUSY);
        //adc_temp[i] = SPI1->RX[0];
		switch (i) {
		  case 0:
              //adc_temp[i] = SPI1->RX[0];
			  adc_data2[cur_phase][adc_data_ready-6] = SPI1->RX[0];   //ch3, A230
		      break;
		  case 1:
              //adc_data[cur_phase][adc_data_ready-6] = SPI1->RX[0];   //ch2, A280
			  adc_temp[i] = SPI1->RX[0];
		      break;
		  case 2:
		      adc_data1[cur_phase][adc_data_ready-6] = SPI1->RX[0];  //ch1, A260
		      break;
		  case 3:
		      //adc_data2[cur_phase][adc_data_ready-6] = SPI1->RX[0];  //ch0, A230
			  adc_data[cur_phase][adc_data_ready-6] = SPI1->RX[0];  //ch0, A280
		      break;
		}
        i++;
      } while (i < 4);
	  if ((adc_data[cur_phase][adc_data_ready-6] < 100 || adc_data1[cur_phase][adc_data_ready-6] < 100 || adc_data2[cur_phase][adc_data_ready-6] < 100)) {
        zero_count++;
//	    printf("%d\n", zero_count);
	  }
	}
	adc_data_ready++;
  }
}

void SysTimerDelay(uint32_t us)
{
  SysTick->LOAD = us * 48; /* Assume the PLL 48MHz */
  SysTick->VAL = (0x00);
  SysTick->CTRL = (1 << SYSTICK_CLKSOURCE) | (1 << SYSTICK_ENABLE);

  /* Waiting for down-count to zero */
  while ((SysTick->CTRL &(1 << 16)) == 0);
}

struct MN913A_setting_type mn913a_preference = { 0 };
void main ( void )
{
  MN913A_init ( );
  Init_Interface_IO ( );
  MaestroNano_Init ( );

  printf ( "MN-913A\n" );
  //SYS->GPBMFP.INT0_SS31 	=0;
  
  /*20160111 added by michael*/
  while ( 1 ) {
    //getchar ();
	if ( recv_cmd == HID_CMD_MN913A_SETTING ) {
	    printf ( "AD5259 write RDAC/EEPROM: %d\n", mn913a_preference.Xenon_Voltage_Level );
		Set_AD5259_Potential ( AD5259_Word_Addr_RDAC, mn913a_preference.Xenon_Voltage_Level );
		SysTimerDelay( 10 );
		Set_AD5259_Potential ( AD5259_Word_Addr_EEPROM, mn913a_preference.Xenon_Voltage_Level );
		SysTimerDelay( 10 );
		recv_cmd = 0;

		if ( !Get_AD5259_Potential ( AD5259_Word_Addr_RDAC, &mn913a_preference.Xenon_Voltage_Level ) )
			printf ( "AD5259 read RDAC: %d\n", mn913a_preference.Xenon_Voltage_Level );
		if ( !Get_AD5259_Potential ( AD5259_Word_Addr_EEPROM, &mn913a_preference.Xenon_Voltage_Level ) )
			printf ( "AD5259 read EEPROM: %d\n", mn913a_preference.Xenon_Voltage_Level );
	}
	else
		if ( recv_cmd == HID_CMD_MN913A_MEASURE ) {
			MaestroNano_Measure ( );		
			recv_cmd = 0;
		}
	//MaestroNano_Measure ( );
    SysTimerDelay ( 10 );
  }
}