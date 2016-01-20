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