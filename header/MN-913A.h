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
	int Illumination_State;
	int start_calibration;
	int Auto_Measure;
	int Reset_MCU;
	int max_voltage_level, min_voltage_level, has_calibration;
	int max_voltage_intensity, min_voltage_intensity;
};

extern uint8_t recv_cmd;
extern  struct MN913A_setting_type mn913a_preference;

#define HID_CMD_MN913A_SETTING 0x86
#define HID_CMD_MN913A_MEASURE 0x87
#define HID_CMD_MN913A_RAW_DATA 0x88
#define HID_CMD_MN913A_STATUS 0x89
#define HID_CMD_PRINT_DNA_RESULT 0x90
#define HID_CMD_PRINT_PROTEIN_RESULT 0x91
#define HID_CMD_GET_TIME 0x92
#define HID_CMD_SET_TIME 0x93
#define HID_CMD_SET_LCD_BRIGHTNESS 0x95
#define HID_CMD_PRINT_META_DATA 0xC1
#define HID_CMD_MN913_FW_UPGRADE 0xA0   //20160705 jan
#define HID_CMD_MN913_FW_HEADER  0xA1   //20160705 jan
#define HID_CMD_PRINTER_POWER_ON  0xA2
#define HID_CMD_PRINTER_POWER_OFF  0xA3

#define MAX_XENON_LEVEL 255
#define MIN_XENON_LEVEL 0

/* I2C variable resistor device */
#define AD5259_I2C_Slave_Addr 0x18
#define AD5259_Word_Addr_RDAC 0x00
#define AD5259_Word_Addr_EEPROM 0x01
#define AD5259_Word_Addr_WP 0x02
#define AD5259_Word_Addr_Sync_EEPROM_RDAC 0x05
#define AD5259_Word_Addr_Sync_RDAC_EEPROM 0x06

/*20160318 added by michael*/
struct MN913A_status_type {
	BOOL remain_in_measure;
	int max_voltage_level, min_voltage_level;
	int max_voltage_intensity, min_voltage_intensity;
	int has_calibration, auto_measure, invalid_measure;
};
extern  struct MN913A_status_type mn913a_status;

/* 20160325 added by michael */
#define Illumination_LED_ON_State 1
#define Illumination_LED_OFF_State 0
struct MN913A_datetime_type {
	int year, month, dayofmonth;
	int hour, minute, second;
};
struct MN913A_dna_data_type {
	int index;
	double conc, A260, A260_A230, A260_A280, A230, A280;
};
struct MN913A_dna_result_type {
  int type, count;
	struct MN913A_datetime_type datetime;
	struct MN913A_dna_data_type dna_data [ 100 ];
};

struct MN913A_protein_data_type {
	int index;
	double A280;
};
struct MN913A_protein_result_type {
  int count;
	struct MN913A_datetime_type datetime;
	struct MN913A_protein_data_type protein_data [ 100 ];
};

extern struct MN913A_dna_result_type mn913a_dna_result_data;
extern struct MN913A_protein_result_type mn913a_protein_result_data;

#define Xenon_Min_Voltag 3.17
#define Xenon_Min_Voltag_index 162
#define Xenon_Max_Voltag 4.7
#define Xenon_Max_Voltag_index 242

#define Target_Generic_A260_Intensity 850000
#define Target_Lowest_A260_Intensity 350000
void Construct_IV_table ();
int Search_Target_Intensity ( int target_intensity );
double Set_Voltage_Get_New_Intensity ( int voltag_level );

void print_dna_result ();
void print_protein_result ();

#define TFT_Send_Command(TFT_Send_Command, len, delay) \
   {  \
     Delay100ms(1); \
     DrvUART_Write(TFT_COMM_PORT, (uint8_t *)TFT_Send_Command, len); \
   }
	 
struct PackedDate {
						unsigned int Second: 6;		// 0..59

						// higher 32 bits
                        unsigned int Minute : 6;    // 0..59                       63 is reserved as EOD marker
                        unsigned int Hour : 5;      // 0..23                       31 is reserved as EOD marker
                        unsigned int Day : 5;       // 1..31
                        unsigned int Month : 4;     // 1..12
                        unsigned int Year : 12;		// 0..4095
                                             
                    };

// 8 byte (64 bit) date time stamp
union MaestroDate
{
						char	Date[5];
						struct PackedDate PackDate;
};

/**/
#define M41T0_I2C_Slave_Addr 0x68

/*struct MN913A_datetime_type {
	int year, month, dayofmonth;
	int hour, minute, second;
};*/
extern struct MN913A_datetime_type mn913a_datetime_data;
extern union MaestroDate temp_date, temp_date1;
extern int mn913a_lcd_brightness;

struct MN913A_normalization_data_type {
	int index;
	double conc, sample_volumn, buffer_volumn;
};

union meta_print_type {
	int type_id;
	struct {
		int print_type_id;
		struct MN913A_datetime_type datetime;
		double before, after;
		int pass_or_fail;
  } cali_print_data;
	
	struct {
		int print_type_id, count;
		double target_volumn, target_conc;
		struct MN913A_normalization_data_type normalization_data [ 10 ];
	} normalization_data;
};

extern union meta_print_type meta_print_data;
void print_meta_data ( union meta_print_type *pmeta_data );
/*20160705 jan*/
struct FW_Header {
  int Version_Code;
  char Version_Name[48];
  int bin_size;
  int HW_Version_Code;
  unsigned char check_digits[16];
};

extern struct FW_Header updated_fw_header;
#define updated_fw_header_base 0x1FE00	
#define updated_fw_bin_base 0x10000
#endif