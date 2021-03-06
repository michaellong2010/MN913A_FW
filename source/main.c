#include <MN-913A.h>

int voltage_index_lower_bound = 0, voltage_index_upper_bound = 0;
volatile uint8_t bPrinter_status;
char buf2[100];
int first_time_boot = 0, lcd_on_event = 0;
void printer_test();
void Delay100ms(int a);
int allow_detect_pmic = 0;

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
		while(UART1->ISR.RDA_IF==1 || UART1->ISR.TOUT_IF==1)
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
  if (u32GpcStatus & (1 << 6)) {  //power_key
	  DrvGPIO_SetBit ( GPB, 11 );
  }
	
  if (u32GpcStatus & (1 << 15) && allow_detect_pmic==1) {
		printf ( "pmic standby detect\n" );
	  if ( DrvGPIO_GetBit ( GPC, 15 ) == 1 )
		  DrvGPIO_ClrBit ( GPC, 14 );
	  else {
			//first_time_boot++;
			//lcd_on_event = 1;
			DrvGPIO_SetBit ( GPC, 14 );
	  }
		  //DrvGPIO_SetBit ( GPC, 14 );
  }
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
  if ((u32GpaStatus & 0x2000) && (adc_data_ready < (Measure_Count+6))) {
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
			  adc_data[cur_phase][adc_data_ready-6] = SPI1->RX[0];   //ch0, A280
		      break;
		  case 1:              
			  //adc_temp[i] = SPI1->RX[0];
			  adc_data3[cur_phase][adc_data_ready-6] = SPI1->RX[0];   //ch3, A320
		      break;
		  case 2:
			  adc_data2[cur_phase][adc_data_ready-6] = SPI1->RX[0];   //ch2, A230
		      break;
		  case 3:
			  adc_data1[cur_phase][adc_data_ready-6] = SPI1->RX[0];  //ch1, A260
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
	else
		if ( (u32GpaStatus & 0x4000) ) {  //GPA.14 interrupt
			if ( mn913a_preference.Auto_Measure == 1 && recv_cmd == 0 ) {
			printf ( "Auto measure detected!\n" );
			/*while ( DrvTIMER_GetTicks(TMR2) <= 2 ) {
			}
			Xenon_PWR_ON ( );
			Illumination_LED_OFF ( );
			Measure_Count = 102;
			MaestroNano_Measure ( );
			if ( mn913a_preference.Illumination_State == Illumination_LED_ON_State )
				Illumination_LED_ON ( )
			else
				Illumination_LED_OFF ( )
			Xenon_PWR_OFF ( );*/
			  //recv_cmd = HID_CMD_MN913A_MEASURE;
				mn913a_status.auto_measure = 1;
				DrvTIMER_ResetTicks(TMR2);
			}
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

struct MN913A_setting_type mn913a_preference = { 0, Illumination_LED_OFF_State, 0, 0, 0 };
struct MN913A_status_type mn913a_status = { 1, 101, 25, 38, 49, 0, 0, 0 };
struct MN913A_dna_result_type mn913a_dna_result_data = { 0 };
struct MN913A_protein_result_type mn913a_protein_result_data = { 0 };
struct MN913A_datetime_type mn913a_datetime_data = { 2016, 6, 14, 13, 35, 56 };
union MaestroDate temp_date, temp_date1;
int mn913a_lcd_brightness = 48;
union meta_print_type meta_print_data = { 0 };
void main ( void )
{
  int i = 0;
  char buf[10];

  temp_date.PackDate.Second = 25;
  temp_date.PackDate.Minute = 27;
  temp_date.PackDate.Hour = 7;
  temp_date.PackDate.Day = 11;
  temp_date.PackDate.Month = 1;
  temp_date.PackDate.Year = 2001;

  MN913A_init ( );
  Init_Interface_IO ( );
  MaestroNano_Init ( );

  printf ( "MN-913A\n" );
  //SYS->GPBMFP.INT0_SS31 	=0;

  /* 20160318 added by michael */
  DrvTIMER_Init();
  DrvTIMER_Open(TMR2,1,PERIODIC_MODE);
  DrvTIMER_EnableInt(TMR2);

  if ( DrvGPIO_ClrBit ( GPE, 5) == 0 )
		printf ( " printer off ");
	else
		printf ( " printer on ");
  /*DrvGPIO_Open(GPC, 3, IO_OUTPUT);
	DrvGPIO_ClrBit( GPC, 3 );
	DrvTIMER_ResetTicks(TMR2);
	while ( DrvTIMER_GetTicks(TMR2) <= 17 ) {
		SysTimerDelay( 10 );
	}*/
	//DrvGPIO_Open(GPC, 3, IO_OUTPUT);
	//DrvGPIO_ClrBit( GPC, 3 );
	/*SysTimerDelay( 5000 );
	DrvGPIO_SetBit( GPC, 3 );*/
	
		/*DrvTIMER_ResetTicks(TMR2);
		while ( DrvTIMER_GetTicks(TMR2) <= 5 ) {
			SysTimerDelay( 10 );
		}
		DrvGPIO_SetBit ( GPC, 14 );
		
		DrvTIMER_ResetTicks(TMR2);
		while ( DrvTIMER_GetTicks(TMR2) <= 13 ) {
			SysTimerDelay( 10 );
		}
		DrvGPIO_SetBit ( GPC, 14 );*/
  //while ( 1 ) {
    //getchar ( );
	  //printer_test ();
	//return;
  //}
	//brightness: F1 04 60 1F 83 F4
	//buf2[0] = 0xF1; buf2[1] = 0x04; buf2[2] = 0x60; buf2[3] = 0x00; buf2[4] = 0x83, buf2[5] = 0xF4;
	//buf2[4] = buf2[1] + buf2[2] + buf2[3];
  //DrvUART_Write(PRINTER_PORT, buf2, 5);
	//while ( 1 )
	//TFT_Send_Command ( buf2, 6, 0 );
	//ex : F1 04 66 09 73 F4 ? Dimmer=9
	//buf2[0] = 0xF1; buf2[1] = 0x04; buf2[2] = 0x66; buf2[3] = 0x09; buf2[4] = 0x73, buf2[5] = 0xF4;
	//buf2[4] = buf2[1] + buf2[2] + buf2[3];
	//TFT_Send_Command ( buf2, 6, 0 );
	
	printf ( "%d\n", sizeof (meta_print_data) );
	printf ( "%d\n", &meta_print_data.normalization_data.print_type_id );
	printf ( "%d\n", &meta_print_data.normalization_data.count );
	printf ( "%d\n", &meta_print_data.normalization_data.target_volumn );
	printf ( "%d\n", &meta_print_data.normalization_data.target_conc );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 0 ].index );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 0 ].conc );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 0 ].sample_volumn );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 0 ].buffer_volumn );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 1 ].index );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 1 ].conc );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 1 ].sample_volumn );
	printf ( "%d\n", &meta_print_data.normalization_data.normalization_data[ 1 ].buffer_volumn );

  printf ( "%d\n", sizeof (meta_print_data) );
	printf ( "%d\n", &meta_print_data.cali_print_data.print_type_id );
	printf ( "%d\n", &meta_print_data.cali_print_data.datetime );
	printf ( "%d\n", &meta_print_data.cali_print_data.datetime.year );
	printf ( "%d\n", &meta_print_data.cali_print_data.datetime.month );
	printf ( "%d\n", &meta_print_data.cali_print_data.datetime.dayofmonth );
	printf ( "%d\n", &meta_print_data.cali_print_data.datetime.hour );
	printf ( "%d\n", &meta_print_data.cali_print_data.datetime.minute );
	printf ( "%d\n", &meta_print_data.cali_print_data.datetime.second );
	printf ( "%d\n", &meta_print_data.cali_print_data.before );
	printf ( "%d\n", &meta_print_data.cali_print_data.after );
	printf ( "%d\n", &meta_print_data.cali_print_data.pass_or_fail );
	
	printf ( "%d\n", sizeof (double) );
	printf ( "%d\n",  &mn913a_protein_result_data.count );
	printf ( "%d\n",  &mn913a_protein_result_data.protein_data );
	printf ( "%d\n",  &mn913a_protein_result_data.datetime );
	printf ( "%d\n", &mn913a_protein_result_data.protein_data[i].index );
	printf ( "%d\n", &mn913a_protein_result_data.protein_data[i].A280 );
	printf ( "%d\n", &mn913a_protein_result_data.protein_data[1].index );
	printf ( "%d\n", &mn913a_protein_result_data.protein_data[1].A280 );
	printf ( "%d\n", &mn913a_protein_result_data.protein_data[2].index );
	printf ( "%d\n", &mn913a_protein_result_data.protein_data[2].A280 );
	
  printf ( "%d\n", sizeof (double) );
  printf ( "%d\n",  &mn913a_dna_result_data.type );
	printf ( "%d\n",  &mn913a_dna_result_data.count );
	printf ( "%d\n",  &mn913a_dna_result_data.datetime );
	printf ( "%d\n",  &mn913a_dna_result_data.dna_data );
	printf ( "%d\n",  &mn913a_dna_result_data.dna_data[0].index );
	printf ( "%d\n",  &mn913a_dna_result_data.dna_data[0].conc );
	printf ( "%d\n",  &mn913a_dna_result_data.dna_data[1].index );
	printf ( "%d\n",  &mn913a_dna_result_data.dna_data[1].conc );
	printf ( "%d\n",  &mn913a_dna_result_data.dna_data[2].index );
	printf ( "%d\n",  &mn913a_dna_result_data.dna_data[2].conc );
	
	Illumination_LED_OFF ( );
	//for ( i = 0; i < 6; i++ )
	   //Set_Maestro_RTC ( &temp_date, i );
	memset ( &temp_date1, 0, sizeof ( union MaestroDate ) );
	Get_Maestro_RTC ( &temp_date1, i);
	mn913a_datetime_data.year = temp_date1.PackDate.Year;
	mn913a_datetime_data.month = temp_date1.PackDate.Month;
	mn913a_datetime_data.dayofmonth = temp_date1.PackDate.Day;
	mn913a_datetime_data.hour = temp_date1.PackDate.Hour;
	mn913a_datetime_data.minute = temp_date1.PackDate.Minute;
	mn913a_datetime_data.second = temp_date1.PackDate.Second;
	printf("Y/M/D H:M:S=%d/%d/%d %d:%d:%d", temp_date1.PackDate.Year, temp_date1.PackDate.Month, temp_date1.PackDate.Day, temp_date1.PackDate.Hour, temp_date1.PackDate.Minute, temp_date1.PackDate.Second );
	//for ( i = 0; i < 10; i++ ) {
		//printf ( "%d\n", &mn913a_dna_result_data.dna_data[ i ] );
  //}
  /*20160111 added by michael*/
	recv_cmd = HID_CMD_MN913A_SETTING;
	mn913a_preference.Xenon_Voltage_Level = 162;
  while ( 1 ) {
    //getchar ();
		//MaestroNano_Measure ( );
		/*if ( mn913a_status.auto_measure == 1 ) {
			if ( DrvTIMER_GetTicks(TMR2) >= 3 )
			  mn913a_status.auto_measure = 0;
    }*/
#if 1
	if ( recv_cmd == HID_CMD_MN913A_SETTING ) {
	  printf ( "AD5259 write RDAC/EEPROM: %d\n", mn913a_preference.Xenon_Voltage_Level );
		if ( HID_CMD_MN913A_SETTING == recv_cmd )
		recv_cmd = 0;
		Set_AD5259_Potential ( AD5259_Word_Addr_RDAC, mn913a_preference.Xenon_Voltage_Level );
		SysTimerDelay( 10 );
		Set_AD5259_Potential ( AD5259_Word_Addr_EEPROM, mn913a_preference.Xenon_Voltage_Level );
		SysTimerDelay( 10 );
		if ( HID_CMD_MN913A_SETTING == recv_cmd )
		recv_cmd = 0;

		if ( !Get_AD5259_Potential ( AD5259_Word_Addr_RDAC, &mn913a_preference.Xenon_Voltage_Level ) )
			printf ( "AD5259 read RDAC: %d\n", mn913a_preference.Xenon_Voltage_Level );
		if ( !Get_AD5259_Potential ( AD5259_Word_Addr_EEPROM, &mn913a_preference.Xenon_Voltage_Level ) )
			printf ( "AD5259 read EEPROM: %d\n", mn913a_preference.Xenon_Voltage_Level );

		if ( mn913a_preference.Illumination_State == Illumination_LED_ON_State ) {
			Illumination_LED_ON()
			//mn913a_preference.Illumination_State = Illumination_LED_ON_State;
		}
		else {
			Illumination_LED_OFF()
			//mn913a_preference.Illumination_State = Illumination_LED_OFF_State;
		}
		
		if ( mn913a_preference.Reset_MCU == 1 ) {
			mn913a_status.has_calibration = 0;
			allow_detect_pmic = 1;
			//DrvSYS_UnlockKeyAddr ();
			//DrvSYS_ResetCPU ();
		}

    if ( mn913a_preference.Auto_Measure == 0 ) {
			mn913a_status.auto_measure = 0;
    }
		if ( mn913a_preference.start_calibration == 1 ) {
			mn913a_status.has_calibration = 0;
			Construct_IV_table ();
			mn913a_status.has_calibration = 1;
			recv_cmd = HID_CMD_MN913A_MEASURE;
			mn913a_status.remain_in_measure = 1;
			//printf("#####  %s %d",__FUNCTION__, mn913a_status.remain_in_measure);//jan
			continue;
    }
		//mn913a_status.auto_measure = mn913a_preference.Auto_Measure;
		if ( HID_CMD_MN913A_SETTING == recv_cmd )
		recv_cmd = 0;
	}
	else
		if ( recv_cmd == HID_CMD_MN913A_MEASURE ) {
			printf ( "comsume command HID_CMD_MN913A_MEASURE\n" );
			mn913a_status.auto_measure = 0;
			//getchar ();
			//SysTimerDelay ( 10 );
			Xenon_PWR_ON ( );
			Illumination_LED_OFF ( );
			Measure_Count = 52;
			mn913a_status.invalid_measure = 0;
			MaestroNano_Measure ( );
			if ( mn913a_preference.Illumination_State == Illumination_LED_ON_State )
				Illumination_LED_ON ( )
			else
				Illumination_LED_OFF ( )
			Xenon_PWR_OFF ( );
			mn913a_status.remain_in_measure = 0;
			//printf("\n");//jan
			//printf("measure finish :::: %s %d",__FUNCTION__, mn913a_status.remain_in_measure);//jan
			//printf("\n");//jan
			recv_cmd = 0;
		}
		else
		   if ( recv_cmd == HID_CMD_MN913A_STATUS ) {
				 printf ( "comsume command HID_CMD_MN913A_STATUS\n" );
			 }
			 else
				  if ( recv_cmd == HID_CMD_PRINT_DNA_RESULT ) {
						printf ( "comsume command HID_CMD_PRINT_DNA_RESULT\n" );
#ifdef PRINTER_PORT
						//DrvGPIO_ClrBit(GPE, 5);
						print_dna_result ( );
						//DrvGPIO_SetBit(GPE, 5);
#endif
						recv_cmd = 0;
					}
					else
						 if ( recv_cmd == HID_CMD_PRINT_PROTEIN_RESULT ) {
							 printf ( "comsume command HID_CMD_PRINT_PROTEIN_RESULT\n" );
#ifdef PRINTER_PORT
							 //DrvGPIO_ClrBit(GPE, 5);
							 print_protein_result ( );
							 //DrvGPIO_SetBit(GPE, 5);
#endif
							 recv_cmd = 0;
						 }
						 else
							 if ( recv_cmd == HID_CMD_SET_TIME ) {
								 temp_date.PackDate.Year = mn913a_datetime_data.year;
								 temp_date.PackDate.Month = mn913a_datetime_data.month;
								 temp_date.PackDate.Day = mn913a_datetime_data.dayofmonth;
								 temp_date.PackDate.Hour = mn913a_datetime_data.hour;
								 temp_date.PackDate.Minute = mn913a_datetime_data.minute;
								 temp_date.PackDate.Second = mn913a_datetime_data.second;
								 for ( i = 0; i < 6; i++ )
									 Set_Maestro_RTC ( &temp_date, i );
								 recv_cmd = 0;
							 }
							 else
								 if ( recv_cmd == HID_CMD_SET_LCD_BRIGHTNESS ) {
									 //brightness: F1 04 60 1F 83 F4
									 /*buf2[0] = 0xF1; buf2[1] = 0x04; buf2[2] = 0x60; buf2[3] = 0x00; buf2[4] = 0x83, buf2[5] = 0xF4;
									 buf2[3] = (char) mn913a_lcd_brightness;
									 buf2[4] = buf2[1] + buf2[2] + buf2[3];*/
									 //ex : F1 06 12 1F 1F 03 59 F4 ? Brightness=31, Contrast=31, Sharpness=3
									 buf2[0] = 0xF1; buf2[1] = 0x06; buf2[2] = 0x12; buf2[3] = 0x1F; buf2[4] = 0x1F, buf2[5] = 0x03; buf2[6] = 0x59; buf2[7] = 0xF4;
									 buf2[3] = (char) mn913a_lcd_brightness;
									 buf2[6] = buf2[1] + buf2[2] + buf2[3] + buf2[4] + buf2[5];
									 TFT_Send_Command ( buf2, 8, 0 );
									 recv_cmd = 0;
								 }
								 else
									 if ( recv_cmd == HID_CMD_PRINT_META_DATA ) {
										 if ( meta_print_data.type_id == 1 ) { //print calibration data
											 printf ( "meta type: %d\n", meta_print_data.cali_print_data.print_type_id );
											 printf ( "year: %d\n", meta_print_data.cali_print_data.datetime.year );
											 printf ( "month: %d\n", meta_print_data.cali_print_data.datetime.month );
											 printf ( "monthofday: %d\n", meta_print_data.cali_print_data.datetime.dayofmonth );
											 printf ( "hour: %d\n", meta_print_data.cali_print_data.datetime.hour );
											 printf ( "minute: %d\n", meta_print_data.cali_print_data.datetime.minute );
											 printf ( "second: %d\n", meta_print_data.cali_print_data.datetime.second );
											 printf ( "before: %4.2f\n", meta_print_data.cali_print_data.before );
											 printf ( "after: %4.2f\n", meta_print_data.cali_print_data.after );
											 printf ( "pass_or_fail: %d\n", meta_print_data.cali_print_data.pass_or_fail );
#ifdef PRINTER_PORT
											 print_meta_data ( &meta_print_data );
#endif
										 }
										 else
											 if ( meta_print_data.type_id == 2 ) { //print normalization data
												 printf ( "meta type: %d\n", meta_print_data.normalization_data.print_type_id );
												 printf ( "normalization data count: %d\n", meta_print_data.normalization_data.count );
												 printf ( "target volumn: %4.2lf\n", meta_print_data.normalization_data.target_volumn );
												 printf ( "target conc: %4.2lf\n", meta_print_data.normalization_data.target_conc );

												 for ( i = 0; i < meta_print_data.normalization_data.count; i++ ) {
													 printf ( "index: %d\n", meta_print_data.normalization_data.normalization_data[i].index );
													 printf ( "conc.: %4.2lf\n", meta_print_data.normalization_data.normalization_data[i].conc );
													 printf ( "sample_volumn: %4.2lf\n", meta_print_data.normalization_data.normalization_data[i].sample_volumn );
													 printf ( "buffer_volumn: %4.2lf\n", meta_print_data.normalization_data.normalization_data[i].buffer_volumn );
												 }
#ifdef PRINTER_PORT
												 print_meta_data ( &meta_print_data );
#endif
											 }
										 recv_cmd = 0;
									 }
									 else
										 if ( recv_cmd == HID_CMD_PRINTER_POWER_ON ) {
											 DrvGPIO_SetBit(GPE, 5);
											 SysTimerDelay ( 10 );
											   //if ( DrvGPIO_ClrBit ( GPE, 5) == 0 )
		//printf ( " printer off ");
	//else
		printf ( " printer on ");
											 recv_cmd = 0;
										 }
										 else
											 if ( recv_cmd == HID_CMD_PRINTER_POWER_OFF ) {
												 DrvGPIO_ClrBit(GPE, 5);
												 SysTimerDelay ( 10 );
												   //if ( DrvGPIO_ClrBit ( GPE, 5) == 0 )
		printf ( " printer off ");
	//else
		//printf ( " printer on ");
												 recv_cmd = 0;
											 }

	//MaestroNano_Measure ( );
    SysTimerDelay ( 10 );
#endif
	/*if ( first_time_boot == 1 && lcd_on_event == 1) {
		DrvTIMER_ResetTicks(TMR2);
		while ( DrvTIMER_GetTicks(TMR2) <= 15 ) {
			SysTimerDelay( 10 );
		}
		DrvGPIO_SetBit ( GPC, 14 );
		lcd_on_event = 0;
	}
	else
		if ( first_time_boot > 1 && lcd_on_event == 1) {
			DrvTIMER_ResetTicks(TMR2);
			while ( DrvTIMER_GetTicks(TMR2) <= 5 ) {
				SysTimerDelay( 10 );
			}

			DrvGPIO_SetBit ( GPC, 14 );
			lcd_on_event = 0;
		}*/
  }
}

void Delay100ms(int a) {
  int i = 0;

  for (i = 0; i < (a*100); i++)
      SysTimerDelay(1000);
}

#ifdef PRINTER_PORT
void printer_test() {
  buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x01;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x03;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x01; buf2[3] = 0xfe; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x02; buf2[3] = 0xf9; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);


  sprintf(buf2, " michael is a good man, good husband, good father\n\r");
  sprintf(buf2+strlen(buf2), " he full with love to family\n\r");
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));

  sprintf(buf2, "\n\r\n\r");
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));

  //buf2[0] = 0x1b; buf2[1] = 0x4a; buf2[2] = 0x05; buf2[3] = 0x00;
  //DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));

  sprintf(buf2, "\n\r\n\r");
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
	
	buf2[0] = 0x1d; buf2[1] = 0x56; buf2[2] = 0x30; buf2[3] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2)+1);
}

void print_meta_data ( union meta_print_type *pmeta_data ) {
  int i = 0;
	
  buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x01;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x03;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x01; buf2[3] = 0xfe; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x02; buf2[3] = 0xf9; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);

  											 /*printf ( "meta type: %d\n", meta_print_data.cali_print_data.print_type_id );
											 printf ( "year: %d\n", meta_print_data.cali_print_data.datetime.year );
											 printf ( "month: %d\n", meta_print_data.cali_print_data.datetime.month );
											 printf ( "monthofday: %d\n", meta_print_data.cali_print_data.datetime.dayofmonth );
											 printf ( "hour: %d\n", meta_print_data.cali_print_data.datetime.hour );
											 printf ( "minute: %d\n", meta_print_data.cali_print_data.datetime.minute );
											 printf ( "second: %d\n", meta_print_data.cali_print_data.datetime.second );
											 printf ( "before: %4.2f\n", meta_print_data.cali_print_data.before );
											 printf ( "after: %4.2f\n", meta_print_data.cali_print_data.after );
											 printf ( "pass_or_fail: %d\n", meta_print_data.cali_print_data.pass_or_fail );
											 print_meta_data ( &meta_print_data );*/
  switch ( meta_print_data.type_id ) {
    case 1:
		strcpy(buf2, "=======Calibration Result=======\n\r");
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, " Date %04d/%02d/%02d\n\r", meta_print_data.cali_print_data.datetime.year, meta_print_data.cali_print_data.datetime.month, meta_print_data.cali_print_data.datetime.dayofmonth );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, " Time %02d:%02d:%02d\n\r", meta_print_data.cali_print_data.datetime.hour, meta_print_data.cali_print_data.datetime.minute, meta_print_data.cali_print_data.datetime.second );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "================================\n\r");
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		
		sprintf(buf2, "before calibration: %4.2f\n\r", meta_print_data.cali_print_data.before );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "after calibration: %4.2f\n\r", meta_print_data.cali_print_data.after );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));

		if ( meta_print_data.cali_print_data.pass_or_fail == 1 )
			sprintf(buf2, "calibration result: pass\n\r");
		else
			sprintf(buf2, "calibration result: fail\n\r");
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		break;
/*
												 printf ( "meta type: %d\n", meta_print_data.normalization_data.print_type_id );
												 printf ( "normalization data count: %d\n", meta_print_data.normalization_data.count );
												 printf ( "target volumn: %4.2lf\n", meta_print_data.normalization_data.target_volumn );
												 printf ( "target conc: %4.2lf\n", meta_print_data.normalization_data.target_conc );

												 for ( i = 0; i < meta_print_data.normalization_data.count; i++ ) {
													 printf ( "index: %d\n", meta_print_data.normalization_data.normalization_data.index );
													 printf ( "conc.: %4.2lf\n", meta_print_data.normalization_data.normalization_data.conc );
													 printf ( "sample_volumn: %4.2lf\n", meta_print_data.normalization_data.normalization_data.sample_volumn );
													 printf ( "buffer_volumn: %4.2lf\n", meta_print_data.normalization_data.normalization_data.buffer_volumn );
												 }
*/
	case 2:
		strcpy(buf2, "======Normalization Result======\n\r");
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "sample count: %d\n\r", meta_print_data.normalization_data.count );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "target volumn: %4.2lf\n\r", meta_print_data.normalization_data.target_volumn );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "target conc.: %4.2lf\n\r", meta_print_data.normalization_data.target_conc );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "================================\n\r");
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));

		for ( i = 0; i < meta_print_data.normalization_data.count; i++ ) {
			sprintf( buf2, "index: %d\n\r", meta_print_data.normalization_data.normalization_data[i].index );
			DrvUART_Write( PRINTER_PORT, buf2, strlen( buf2 ) );

			sprintf( buf2, "Conc.: %4.2lf\n\r", meta_print_data.normalization_data.normalization_data[i].conc );
			DrvUART_Write( PRINTER_PORT, buf2, strlen( buf2 ) );

			sprintf( buf2, "sample_volumn: %4.2lf\n\r", meta_print_data.normalization_data.normalization_data[i].sample_volumn );
			DrvUART_Write( PRINTER_PORT, buf2, strlen( buf2 ) );

			sprintf( buf2, "buffer_volumn: %4.2lf\n\r", meta_print_data.normalization_data.normalization_data[i].buffer_volumn );
			DrvUART_Write( PRINTER_PORT, buf2, strlen( buf2 ) );
		}
		break;
  }

  sprintf(buf2, "\n\r\n\r\n\r");
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
  buf2[0] = 0x1b; buf2[1] = 0x4a; buf2[2] = 0x05; buf2[3] = 0x00;  //print & feed forward
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));

  buf2[0] = 0x1d; buf2[1] = 0x56; buf2[2] = 0x30; buf2[3] = 0x00;  //paper cut
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
}

void print_dna_result () {
	int i = 0;

  buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x01;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x03;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x01; buf2[3] = 0xfe; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x02; buf2[3] = 0xf9; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);
	
  if ( mn913a_dna_result_data.count > 0 ) {
		switch ( mn913a_dna_result_data.type ) {
			case 0:
				strcpy(buf2, "==============dsDNA=============\n\r");
				break;
			case 1:
				strcpy(buf2, "==============ssDNA=============\n\r");
				break;
			case 2:
				strcpy(buf2, "===============RNA==============\n\r");
				break;
    }
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		
		/*sprintf(buf2, " Date %04d/%02d/%02d\n\r", pSheet->sheet_date.PackDate.Year, pSheet->sheet_date.PackDate.Month, pSheet->sheet_date.PackDate.Day);
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, " Time %02d:%02d:%02d\n\r", pSheet->sheet_date.PackDate.Hour, pSheet->sheet_date.PackDate.Minute, pSheet->sheet_date.PackDate.Second);
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));*/
		sprintf(buf2, " Date %04d/%02d/%02d\n\r", mn913a_dna_result_data.datetime.year, mn913a_dna_result_data.datetime.month, mn913a_dna_result_data.datetime.dayofmonth );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, " Time %02d:%02d:%02d\n\r", mn913a_dna_result_data.datetime.hour, mn913a_dna_result_data.datetime.minute, mn913a_dna_result_data.datetime.second );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "================================\n\r");
    DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		
		for ( i = 0; i < mn913a_dna_result_data.count; i++ ) {
			sprintf(buf2, "   sample_%d:\n\r", mn913a_dna_result_data.dna_data[i].index);
			DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
			
			//strcpy(buf2, "   A260       Conc.          \n\r");
			strcpy(buf2, "   A230       A260       A280\n\r");
			DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
    
			//sprintf( buf2, "    %2.3f", mn913a_dna_result_data.dna_data[i].A260 );
			sprintf( buf2, "    %2.3f", mn913a_dna_result_data.dna_data[i].A230 );
			/*if ( mn913a_dna_result_data.dna_data[i].A260 < 10)
				sprintf(buf2+strlen(buf2), "      %4.2f (ng/uL)\n\r", mn913a_dna_result_data.dna_data[i].conc );
			else
			   sprintf(buf2+strlen(buf2), "     %4.2f\n\r (ng/uL)", mn913a_dna_result_data.dna_data[i].conc );*/
			if ( mn913a_dna_result_data.dna_data[i].A230 < 10 )
				sprintf( buf2 + strlen(buf2), "      %2.3f", mn913a_dna_result_data.dna_data[i].A260 );
			else
				 sprintf( buf2 + strlen(buf2), "     %2.3f", mn913a_dna_result_data.dna_data[i].A260 );
			if ( mn913a_dna_result_data.dna_data[i].A260 < 10 )
				sprintf( buf2 + strlen(buf2), "      %2.3f\n\r", mn913a_dna_result_data.dna_data[i].A280 );
			else
				 sprintf( buf2 + strlen(buf2), "     %2.3f\n\r", mn913a_dna_result_data.dna_data[i].A280 );
			DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
			
			strcpy(buf2, "   A260/A230       A260/A280\n\r");
			DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
      sprintf(buf2, "    %2.3f           %2.3f\n\r", mn913a_dna_result_data.dna_data[i].A260_A230, mn913a_dna_result_data.dna_data[i].A260_A280 );
		  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
			
			sprintf(buf2, "   Conc. %4.2f (ng/uL)\n\r\n\r", mn913a_dna_result_data.dna_data[i].conc );
			DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		}
	}
	
	sprintf(buf2, "\n\r\n\r\n\r");
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
	buf2[0] = 0x1b; buf2[1] = 0x4a; buf2[2] = 0x05; buf2[3] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
	
	buf2[0] = 0x1d; buf2[1] = 0x56; buf2[2] = 0x30; buf2[3] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
}

void print_protein_result () {
	int i = 0;
	
	buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x01;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x1d; buf2[1] = 0x72; buf2[2] = 0x03;
  DrvUART_Write(PRINTER_PORT, buf2, 3);
  Delay100ms(2);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x01; buf2[3] = 0xfe; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);

  buf2[0] = 0x12; buf2[1] = 0x77; buf2[2] = 0x02; buf2[3] = 0xf9; buf2[4] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, 5);
	
	if ( mn913a_protein_result_data.count > 0 ) {
		strcpy(buf2, "=============Protein============\n\r");
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));

		sprintf(buf2, " Date %04d/%02d/%02d\n\r", mn913a_protein_result_data.datetime.year, mn913a_protein_result_data.datetime.month, mn913a_protein_result_data.datetime.dayofmonth );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, " Time %02d:%02d:%02d\n\r", mn913a_protein_result_data.datetime.hour, mn913a_protein_result_data.datetime.minute, mn913a_protein_result_data.datetime.second );
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		sprintf(buf2, "================================\n\r");
    DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		
		sprintf(buf2, "   sample_%d:\n\r", mn913a_protein_result_data.protein_data[i].index);
		DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		
		for ( i = 0; i < mn913a_protein_result_data.count; i++ ) {
			strcpy(buf2, "   A280");
			DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
      
			sprintf(buf2, "    %2.3f\n\r", mn913a_protein_result_data.protein_data[i].A280);
      DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
			 
			sprintf(buf2, "   Conc.(ref.) %4.2f (mg/mL)\n\r", mn913a_protein_result_data.protein_data[i].A280);
      sprintf(buf2+strlen(buf2), "   Conc.(BSA) %4.2f (mg/mL)\n\r\n\r", mn913a_protein_result_data.protein_data[i].A280*1.52);
			DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
		}
	}
	
	sprintf(buf2, "\n\r\n\r\n\r");
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
	buf2[0] = 0x1b; buf2[1] = 0x4a; buf2[2] = 0x05; buf2[3] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
	
	buf2[0] = 0x1d; buf2[1] = 0x56; buf2[2] = 0x30; buf2[3] = 0x00;
  DrvUART_Write(PRINTER_PORT, buf2, strlen(buf2));
}
#endif

//xenon_voltage = 3.17 + ( 4.7 - 3.17 ) * ( Cur_Voltage_Level - 162 ) / ( 242 - 162 );
void Construct_IV_table (  ) {
	//int voltage_index_lower_bound = 0, voltage_index_upper_bound = 0;
	voltage_index_upper_bound = Search_Target_Intensity ( Target_Generic_A260_Intensity );
	voltage_index_lower_bound = Search_Target_Intensity ( Target_Lowest_A260_Intensity );
	mn913a_status.max_voltage_level = voltage_index_upper_bound;
	mn913a_status.min_voltage_level = voltage_index_lower_bound;
	mn913a_status.min_voltage_intensity = Set_Voltage_Get_New_Intensity ( mn913a_status.min_voltage_level, 0 );
	mn913a_status.max_voltage_intensity = Set_Voltage_Get_New_Intensity ( mn913a_status.max_voltage_level, 0 );
}

double Set_Voltage_Get_New_Intensity ( int voltag_level, int is_calibration ) {
	  double intensity = 0.0;
	  int i = 0;
	
		Set_AD5259_Potential ( AD5259_Word_Addr_RDAC, voltag_level );
		SysTimerDelay( 10 );
		Set_AD5259_Potential ( AD5259_Word_Addr_EEPROM, voltag_level );
		SysTimerDelay( 10 );

		if ( !Get_AD5259_Potential ( AD5259_Word_Addr_RDAC, &voltag_level ) )
			printf ( "AD5259 read RDAC: %d\n", voltag_level );
		if ( !Get_AD5259_Potential ( AD5259_Word_Addr_EEPROM, &voltag_level ) )
			printf ( "AD5259 read EEPROM: %d\n", voltag_level );

		if ( is_calibration == 1 )
			Measure_Count = 12;
		else
			 Measure_Count = 52;
		Xenon_PWR_ON ( );	
		mn913a_status.invalid_measure = 0;
		MaestroNano_Measure ( );
		for ( i = 2; i < Measure_Count; i++ ) {
			intensity += adc_data1[1][i] - adc_data1[0][i];
		  //mean[count2][1] += adc_data1[0][k];
      //mean1[count2][1] += adc_data1[1][k];
		}
		intensity = intensity / ( Measure_Count - 2 );
		printf ( "current intensity: %lf\n", intensity );
		Xenon_PWR_OFF ( );
		
		return intensity;
}

int Search_Target_Intensity ( int target_intensity ) {
  //double xenon_voltage1 = 3.17, xenon_voltage2 = 4.7;
	//double Voltage_Level1 = 162 + ( ( xenon_voltage1 - 3.17 ) * ( 242 -162 ) ) / ( 4.7 - 3.17 );
	//double Voltage_Level2 = 162 + ( ( xenon_voltage2 - 3.17 ) * ( 242 -162 ) ) / ( 4.7 - 3.17 );
	  int cur_intensity = 0, Voltage_Level1 = 162, Voltage_Level2 = 242, best_voltage_level = 0;
	  double intensity1 = 0.0, intensity2 = 0.0, new_target, outofbound = -1;
	
	  intensity1 = Set_Voltage_Get_New_Intensity ( Voltage_Level1, 1 );
	  intensity2 = Set_Voltage_Get_New_Intensity ( Voltage_Level2, 1 );
		//mn913a_status.max_voltage_intensity = intensity1;
	  //mn913a_status.min_voltage_intensity = intensity2;
				
		if ( intensity2 < target_intensity ) {
			printf ( "outof max bound\n" );
			outofbound = 0;
			best_voltage_level = Voltage_Level2;
    }
		else
		   if ( intensity1 > target_intensity ) {
				 printf ( "outof min bound\n" );
				 outofbound = 1;
				 best_voltage_level = Voltage_Level1;
       }
			 else {
				  Illumination_LED_OFF ( );
				 	while ( Voltage_Level1 < Voltage_Level2 ) {
						new_target = Set_Voltage_Get_New_Intensity ( ( Voltage_Level1 + Voltage_Level2 ) / 2, 1 );
						
						if ( new_target < target_intensity ) {
							Voltage_Level1 = ( Voltage_Level1 + Voltage_Level2 ) / 2;
            }
						else {
							 Voltage_Level2 = ( Voltage_Level1 + Voltage_Level2 ) / 2;
            }
						
						if ( ( Voltage_Level2 - Voltage_Level1 ) == 1 )
							break;
          }
					
					best_voltage_level = Voltage_Level2;
       }
    if ( outofbound == -1 ) {
			printf ( "find the best voltage level ( %d, %d )\n", Voltage_Level1, Voltage_Level2 );
			if ( mn913a_preference.Illumination_State == Illumination_LED_ON_State )
			  Illumination_LED_ON ( )
		  else
			   Illumination_LED_OFF ( )
		}
		return best_voltage_level;
		//GetIntensity
}