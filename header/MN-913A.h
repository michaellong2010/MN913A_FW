#ifndef __MN913A_H__
#define __MN913A_H__

#include <stdio.h>
#include "NUC1xx.h"
#include "DrvSYS.h"
#include "DrvUART.h"
#include "DrvTIMER.h"
#include "DrvUSB.h"
#include "DrvGPIO.h"
#include "DrvSPI.h"
#include "DrvPWM.h"
#include "DrvI2C.h"
#include "measurement.h"

#define USE_USB 1
//#define MaestroNano_Fit_Mode 1

#ifdef MaestroNano_Fit_Mode  //fitting process

#define DEBUG_INFO_PORT UART_PORT0
#define TFT_COMM_PORT UART_PORT1
#define DEBUG_INFO_PORT_ID 0
#define TFT_COMM_PORT_ID 1

#else  //release process

#define DEBUG_INFO_PORT UART_PORT2
#define DEBUG_INFO_PORT_ID 2
#define PRINTER_PORT UART_PORT0
#define TFT_COMM_PORT UART_PORT1
#define PRINTER_PORT_ID 0
#define TFT_COMM_PORT_ID 1

#endif //MaestroNano_Fit_Mode

void UART_INT_HANDLE(uint32_t u32IntStatus);
void GPABCallback(uint32_t u32GpaStatus, uint32_t u32GpbStatus);
void GPCDECallback(uint32_t u32GpcStatus, uint32_t u32GpdStatus, uint32_t u32GpeStatus);
void EINT0Callback(void);

#define RTC_Power_ADC 1
#define USE_KEY_QUEUE 1

#define SPI_Clock 400000

#define PWM_Param2 2
#define PWM_Param3 3
#define PWM_Param4 4
#define PWM_Param5 5
#define PWM_Count_Method PWM_Param3

struct MN913A_setting_type {
	int Xenon_Voltage_Level;
};

extern uint8_t recv_cmd;
extern  struct MN913A_setting_type mn913a_preference;

#define HID_CMD_MN913A_SETTING 0x86
#define HID_CMD_MN913A_MEASURE 0x87

#define MAX_XENON_LEVEL 255
#define MIN_XENON_LEVEL 0

/* I2C variable resistor device */
#define AD5259_I2C_Slave_Addr 0x18
#define AD5259_Word_Addr_RDAC 0x00
#define AD5259_Word_Addr_EEPROM 0x01
#define AD5259_Word_Addr_WP 0x02
#define AD5259_Word_Addr_Sync_EEPROM_RDAC 0x05
#define AD5259_Word_Addr_Sync_RDAC_EEPROM 0x06
#endif