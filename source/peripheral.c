#include "MN-913A.h"

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 (Master) Tx Callback Function, I2C0 as transmitter				                                   */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t Tx_Data0[10];
uint8_t Rx_Data0[10];
uint8_t DataLen0, Active_I2C_Slave_Addr;
volatile uint8_t start_I2C, Date_valid;

volatile uint8_t EndFlag0 = 0;
void I2C0_Callback_Tx(uint32_t status)
{
	if (EndFlag0) {
		DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
#ifdef SYS_DEBUG
		printf("Tx EndFlag0: %d\n", EndFlag0);
#endif
	  return;
	}

	if (status == 0x08)						/* START has been transmitted */
	{
	  if (!start_I2C) {
		DataLen0 = 0;
		//DrvI2C_WriteData(I2C_PORT0, M41T0_I2C_Slave_Addr<<1);
		DrvI2C_WriteData(I2C_PORT0, Active_I2C_Slave_Addr<<1);
		DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
#ifdef SYS_DEBUG
		printf("start I2C Tx\n");
#endif
		start_I2C = 1;
	  }
	  else {
         EndFlag0 = 1;
		 DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
	  }
	}	
	else if (status == 0x18)				/* SLA+W has been transmitted and ACK has been received */
	{
		//DrvI2C_WriteData(I2C_PORT0, Tx_Data0[DataLen0++]);
		DrvI2C_WriteData(I2C_PORT0, Tx_Data0[0]);  //word addrsss, 8-bit
		DataLen0++;
		DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
	}
	else if (status == 0x20)				/* SLA+W has been transmitted and NACK has been received */
	{
		
		DrvI2C_Ctrl(I2C_PORT0, 1, 1, 1, 0);
	}	
	else if (status == 0x28)				/* DATA has been transmitted and ACK has been received */
	{
/*		if (DataLen0 != 3)
		{
			DrvI2C_WriteData(I2C_PORT0, Tx_Data0[DataLen0++]);
			DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
		}
		else
		{
			DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
			EndFlag0 = 1;
		}*/
		if (DataLen0 < Tx_Data0[1]) {
		  DrvI2C_WriteData(I2C_PORT0, Tx_Data0[1+DataLen0]);
		  DataLen0++;
		  DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
		}
		else {
		   DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
		   EndFlag0 = 1;
		}
	}
	else
	{
#ifdef SYS_DEBUG
		printf("I2C_Tx Status 0x%x is NOT processed\n", status);
#endif
		EndFlag0 = 1;
	}		
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 (Master) Rx Callback Function									                                   */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_Callback_Rx(uint32_t status)
{
	if (EndFlag0) {
		DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
#ifdef SYS_DEBUG
		printf("Rx EndFlag0: %d\n", EndFlag0);
#endif
	  return;
	}

	if (status == 0x08)					   	/* START has been transmitted and prepare SLA+W */
	{
	  if (!start_I2C) {
        DataLen0 = 0;
		//DrvI2C_WriteData(I2C_PORT0, M41T0_I2C_Slave_Addr<<1);
		DrvI2C_WriteData(I2C_PORT0, Active_I2C_Slave_Addr<<1);
		DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
#ifdef SYS_DEBUG
		printf("start I2C Rx\n");
#endif
		start_I2C = 1;
	  }
	  else {
         EndFlag0 = 1;
		 DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
		 Date_valid = 0;
	  }
	}	
	else if (status == 0x18)				/* SLA+W has been transmitted and ACK has been received */
	{
		//DrvI2C_WriteData(I2C_PORT0, Tx_Data0[DataLen0++]);
		DrvI2C_WriteData(I2C_PORT0, Rx_Data0[0]);  //word addrsss, 8-bit
		DataLen0++;
		DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
	}
	else if (status == 0x20)				/* SLA+W has been transmitted and NACK has been received */
	{
		DrvI2C_Ctrl(I2C_PORT0, 1, 1, 1, 0);
	}
	else if (status == 0x28)				/* DATA has been transmitted and ACK has been received */
	{   //word address has been transmitted
		/*if (DataLen0 != 2)
		{
			DrvI2C_WriteData(I2C_PORT0, Tx_Data0[DataLen0++]);
			DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
		}
		else
		{
			DrvI2C_Ctrl(I2C_PORT0, 1, 0, 1, 0);
		}		*/
		DrvI2C_Ctrl(I2C_PORT0, 1, 0, 1, 0);
	}
	else if (status == 0x10)				/* Repeat START has been transmitted and prepare SLA+R */
	{
		//DrvI2C_WriteData(I2C_PORT0, M41T0_I2C_Slave_Addr<<1 | 0x01);
		DrvI2C_WriteData(I2C_PORT0, Active_I2C_Slave_Addr<<1 | 0x01);
		DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
	}
	else if (status == 0x40)				/* SLA+R has been transmitted and ACK has been received */
	{
		DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
	}		
	else if (status == 0x58)				/* DATA has been received and NACK has been returned */
	{
/*		Rx_Data0 = DrvI2C_ReadData(I2C_PORT0);
		DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
		EndFlag0 = 1;*/
		if (DataLen0 < Rx_Data0[1]) {
		  //for (; DataLen0 < Rx_Data0[1]; DataLen0++)
		     Rx_Data0[1+DataLen0] = DrvI2C_ReadData(I2C_PORT0);
		     DataLen0++;
		  DrvI2C_Ctrl(I2C_PORT0, 0, 0, 1, 0);
		}
		else {
		   DrvI2C_Ctrl(I2C_PORT0, 0, 1, 1, 0);
		   EndFlag0 = 1;
		}
	}
	else
	{
#ifdef SYS_DEBUG
		printf("I2C_Rx Status 0x%x is NOT processed\n", status);
#endif
		EndFlag0 = 1;
		Date_valid = 0;
	}			
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 (Master) Timeout Function									                                   */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_Timeout(uint32_t status)
{
#ifdef SYS_DEBUG
  printf("I2C time out\n");
#endif
  EndFlag0 = 1;
}

//==============================================================================
//Subroutine:	Set_Maestro_RTC
//Function: Set date/time from RTC
//==============================================================================
void Set_Maestro_RTC(union MaestroDate *pDate, int index)
{
//	Set_Maestro_RTC(&temp_date, 10);
/*	   Tx_Data0[2] = (pDate->PackDate.Second / 10) << 4 + (pDate->PackDate.Second % 10);
       Tx_Data0[3] = (pDate->PackDate.Minute / 10) << 4 + (pDate->PackDate.Minute % 10);
	   Tx_Data0[4] = (pDate->PackDate.Hour / 10) << 4 + (pDate->PackDate.Hour % 10);
	   Tx_Data0[5] = 3;
       Tx_Data0[6] = (pDate->PackDate.Day / 10) << 4 + (pDate->PackDate.Day % 10);
       Tx_Data0[7] = (pDate->PackDate.Month / 10) << 4 + (pDate->PackDate.Month % 10);
       Tx_Data0[8] = (pDate->PackDate.Year / 10) << 4 + (pDate->PackDate.Year % 10);*/
	   //Tx_Data0[2] = pDate->PackDate.Second;
	   Tx_Data0[2] = (pDate->PackDate.Second / 10) << 4;
	   Tx_Data0[2] += (pDate->PackDate.Second % 10);
       //Tx_Data0[3] = pDate->PackDate.Minute;
	   Tx_Data0[3] = (pDate->PackDate.Minute / 10) << 4;
	   Tx_Data0[3] += (pDate->PackDate.Minute % 10);
	   //Tx_Data0[4] = pDate->PackDate.Hour;
	   Tx_Data0[4] = (pDate->PackDate.Hour / 10) << 4;
	   Tx_Data0[4] += (pDate->PackDate.Hour % 10);
	   Tx_Data0[5] = 3;
       //Tx_Data0[6] = pDate->PackDate.Day;
	   Tx_Data0[6] = (pDate->PackDate.Day / 10) << 4;
	   Tx_Data0[6] += (pDate->PackDate.Day % 10);
       //Tx_Data0[7] = pDate->PackDate.Month;
	   Tx_Data0[7] = (pDate->PackDate.Month / 10) << 4;
	   Tx_Data0[7] += (pDate->PackDate.Month % 10);
       //Tx_Data0[8] = pDate->PackDate.Year;
	   if (pDate->PackDate.Year < 100) {
	     Tx_Data0[8] = (pDate->PackDate.Year / 10) << 4;
	     Tx_Data0[8] += (pDate->PackDate.Year % 10);
	   }
	   else {
	      Tx_Data0[8] = ((pDate->PackDate.Year-2000) / 10) << 4;
	      Tx_Data0[8] += ((pDate->PackDate.Year-2000) % 10);	   
	   }
	   Tx_Data0[1] = 8;
	   Tx_Data0[0] = 0;
  switch (index) {
	case 0:
	   Tx_Data0[0] = 0;
	   break;
	case 1:
	   Tx_Data0[0] = 1;
	   Tx_Data0[2] = Tx_Data0[3];
	   break;
	case 2:
	   Tx_Data0[0] = 2;
	   Tx_Data0[2] = Tx_Data0[4];
	   break;
	case 3:
	   Tx_Data0[0] = 4;
	   Tx_Data0[2] = Tx_Data0[6];
	   break;
	case 4:
	   Tx_Data0[0] = 5;
	   Tx_Data0[2] = Tx_Data0[7];
	   break;
	case 5:
	   Tx_Data0[0] = 6;
	   Tx_Data0[2] = Tx_Data0[8];
	   break;
  }

  if (index < 6) {
	Tx_Data0[1] = 2;
  }
	
	UNLOCKREG();
	DrvI2C_InstallCallback(I2C_PORT0, I2CFUNC, I2C0_Callback_Tx);
	DrvI2C_InstallCallback(I2C_PORT0, TIMEOUT, I2C0_Timeout);
	DrvI2C_EnableTimeoutCount(I2C_PORT0, 1, 1);
	/* I2C0 as master sends START signal */
	start_I2C = 0;
	EndFlag0 = 0;
	Active_I2C_Slave_Addr = M41T0_I2C_Slave_Addr;
	DrvI2C_Ctrl(I2C_PORT0, 1, 0, 0, 0);
	while (EndFlag0 == 0);
	DrvI2C_UninstallCallBack(I2C_PORT0, I2CFUNC);
	DrvI2C_UninstallCallBack(I2C_PORT0, TIMEOUT);
	DrvI2C_EnableTimeoutCount(I2C_PORT0, 0, 0);
	LOCKREG();
}

//==============================================================================
//Subroutine:	Get_Maestro_RTC
//Function: Get date/time from RTC
//==============================================================================
void Get_Maestro_RTC(union MaestroDate *pDate, int index)
{
  int i;
  UNLOCKREG();
  Rx_Data0[1] = 2;
  DrvI2C_InstallCallback(I2C_PORT0, I2CFUNC, I2C0_Callback_Rx);
  DrvI2C_InstallCallback(I2C_PORT0, TIMEOUT, I2C0_Timeout);
  DrvI2C_EnableTimeoutCount(I2C_PORT0, 1, 1);

  for (i = 5; i >= 0; i--) {
     EndFlag0 = 0;
     if (i > 2)
	   Rx_Data0[0] = i + 1;
	 else
        Rx_Data0[0] = i;
/* I2C0 as master sends START signal */
	 start_I2C = 0;
	 EndFlag0 = 0;
	 Date_valid = 1;
	 Active_I2C_Slave_Addr = M41T0_I2C_Slave_Addr;
     DrvI2C_Ctrl(I2C_PORT0, 1, 0, 0, 0);
	 while (EndFlag0 == 0);
//	     Get_Maestro_RTC(&temp_date, i);
//	  }
	 if (Date_valid) {
  switch(i) {
	case 0:
	   pDate->PackDate.Second = 10 * ((Rx_Data0[2]&0x7f) >> 4) + (Rx_Data0[2]&0x0f);
	   break;
	case 1:
	   pDate->PackDate.Minute = 10 * ((Rx_Data0[2]&0x7f) >> 4) + (Rx_Data0[2]&0x0f);
	   break;
	case 2:
       pDate->PackDate.Hour = 10 * ((Rx_Data0[2]&0x3f) >> 4) + (Rx_Data0[2]&0x0f);
	   break;
	case 3:
	   pDate->PackDate.Day = 10 * ((Rx_Data0[2]&0x3f) >> 4) + (Rx_Data0[2]&0x0f);
	   break;
	case 4:
	   pDate->PackDate.Month = 10 * ((Rx_Data0[2]&0x1f) >> 4) + (Rx_Data0[2]&0x0f);
	   break;
	case 5:
	   pDate->PackDate.Year = 10 * ((Rx_Data0[2]&0xff) >> 4) + (Rx_Data0[2]&0x0f);
	   if (pDate->PackDate.Year < 100)
	     pDate->PackDate.Year += 2000;
	   break;
  }
  }
  }
  DrvI2C_UninstallCallBack(I2C_PORT0, TIMEOUT);
  DrvI2C_UninstallCallBack(I2C_PORT0, I2CFUNC);
  DrvI2C_EnableTimeoutCount(I2C_PORT0, 0, 0);
  LOCKREG();

/*  pDate->PackDate.Second = 10 * ((Rx_Data0[2]&0x7f) >> 4) + (Rx_Data0[2]&0x0f);
  pDate->PackDate.Minute = 10 * ((Rx_Data0[3]&0x7f) >> 4) + (Rx_Data0[3]&0x0f);
  pDate->PackDate.Hour = 10 * ((Rx_Data0[4]&0x3f) >> 4) + (Rx_Data0[4]&0x0f);
  pDate->PackDate.Day = 10 * ((Rx_Data0[6]&0x3f) >> 4) + (Rx_Data0[6]&0x0f);
  pDate->PackDate.Month = 10 * ((Rx_Data0[7]&0x1f) >> 4) + (Rx_Data0[7]&0x0f);
  pDate->PackDate.Year = 10 * ((Rx_Data0[8]&0xff) >> 4) + (Rx_Data0[8]&0x0f);*/
/*const unsigned char gImage_kkk1[32] = { 
0X00,0X00,0X03,0XC0,0X7C,0X00,0X00,0X00,0X00,0X00,0X07,0XE0,0XFC,0X00,0X00,0X00,
0X00,0X00,0X07,0XF0,0XFE,0X00,0X00,0X00,0X00,0X00,0X0F,0XF1,0XFF,0X00,0X00,0X00,
};*/
}

/*20150515 added by michael*/
void Set_AD5259_Potential ( uint8_t word_addr, uint8_t data )
{
	Tx_Data0[0] = word_addr;
	Tx_Data0[1] = 2;
	Tx_Data0[2] = data;

	UNLOCKREG();
	DrvI2C_InstallCallback(I2C_PORT0, I2CFUNC, I2C0_Callback_Tx);
	DrvI2C_InstallCallback(I2C_PORT0, TIMEOUT, I2C0_Timeout);
	DrvI2C_EnableTimeoutCount(I2C_PORT0, 1, 1);
	/* I2C0 as master sends START signal */
	start_I2C = 0;
	EndFlag0 = 0;
	Active_I2C_Slave_Addr = AD5259_I2C_Slave_Addr;
	DrvI2C_Ctrl(I2C_PORT0, 1, 0, 0, 0);
	while (EndFlag0 == 0);
	DrvI2C_UninstallCallBack(I2C_PORT0, I2CFUNC);
	DrvI2C_UninstallCallBack(I2C_PORT0, TIMEOUT);
	DrvI2C_EnableTimeoutCount(I2C_PORT0, 0, 0);
	LOCKREG();
}

int Get_AD5259_Potential ( uint8_t word_addr, uint8_t *pdata )
{
	int result = 0;
	UNLOCKREG();
	Rx_Data0[1] = 2;
	Rx_Data0[0] = word_addr;
	DrvI2C_InstallCallback(I2C_PORT0, I2CFUNC, I2C0_Callback_Rx);
	DrvI2C_InstallCallback(I2C_PORT0, TIMEOUT, I2C0_Timeout);
	DrvI2C_EnableTimeoutCount(I2C_PORT0, 1, 1);

	EndFlag0 = 0;
	start_I2C = 0;
	EndFlag0 = 0;
	Date_valid = 1;
	Active_I2C_Slave_Addr = AD5259_I2C_Slave_Addr;
	DrvI2C_Ctrl(I2C_PORT0, 1, 0, 0, 0);
	while (EndFlag0 == 0);
	if ( Date_valid ) {
		*pdata = Rx_Data0[2];
	}
	else
		result = -1;
	DrvI2C_UninstallCallBack(I2C_PORT0, TIMEOUT);
	DrvI2C_UninstallCallBack(I2C_PORT0, I2CFUNC);
	DrvI2C_EnableTimeoutCount(I2C_PORT0, 0, 0);
	LOCKREG();

	return result;
}