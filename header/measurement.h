#ifndef __Measurement_h
#define __Measurement_h

#include "MN-913A.h"

/* System Clock source, Ref. man pg.155-156 */
#define CONFIG_SYS_Clk_XTL12M 0
#define CONFIG_SYS_Clk_OSC22M 4
#define CONFIG_SYS_Clk_PLLFOUT 2

//#define CONFIG_USING_SYS_Clk CONFIG_SYS_Clk_XTL12M	/* user def. */
#define CONFIG_USING_SYS_Clk CONFIG_SYS_Clk_PLLFOUT
//#define CONFIG_USING_SYS_Clk CONFIG_SYS_Clk_OSC22M

#if CONFIG_USING_SYS_Clk==CONFIG_SYS_Clk_PLLFOUT
#define CONFIG_PLLCON_SRC_XTL12M 0
#define CONFIG_PLLCON_SRC_OSC22M 1
#endif

#define CONFIG_INTERNAL_XENON_CONV_CONTROL 1
#ifdef CONFIG_INTERNAL_XENON_CONV_CONTROL
//#define DDC114_Start_Conversion() DrvPWM_Enable(DRVPWM_TIMER0, 1)
#define DDC114_Start_Conversion() \
  {	\
    DrvPWM_Enable(DRVPWM_TIMER0, 1); \
    while(DrvPWM_GetTimerCounter(DRVPWM_TIMER0)); \
    DrvPWM_Enable(DRVPWM_TIMER0, 0); \
	DrvPWM_Enable(DRVPWM_TIMER0, 1); \
  }
//#define DDC114_Stop_Conversion() DrvPWM_Enable(DRVPWM_TIMER0, 0)
#define DDC114_Stop_Conversion() \
  {	\
    while(DrvPWM_GetTimerCounter(DRVPWM_TIMER0)); \
    DrvPWM_Enable(DRVPWM_TIMER0, 0); \
  }
//#define Xenon_Flash_On() DrvPWM_Enable(DRVPWM_TIMER3, 1)
#define Xenon_Flash_On() \
  { \
    DrvPWM_Enable(DRVPWM_TIMER3, 1); \
    while(DrvPWM_GetTimerCounter(DRVPWM_TIMER3)); \
    DrvPWM_Enable(DRVPWM_TIMER3, 0); \
	DrvPWM_Enable(DRVPWM_TIMER3, 1); \
  }
//#define Xenon_Flash_Off() DrvPWM_Enable(DRVPWM_TIMER3, 0)
#define Xenon_Flash_Off() \
  {	\
    while(DrvPWM_GetTimerCounter(DRVPWM_TIMER3)); \
    DrvPWM_Enable(DRVPWM_TIMER3, 0); \
  }

/* 20160325 added by michael */
#define Xenon_PWR_ON() DrvGPIO_SetBit ( GPB, 10 );
#define Xenon_PWR_OFF() DrvGPIO_ClrBit ( GPB, 10 );
#define Illumination_LED_ON() DrvGPIO_SetBit ( GPB, 9 );
#define Illumination_LED_OFF() DrvGPIO_ClrBit ( GPB, 9 );
#else
#define DDC114_Start_Conversion() DrvGPIO_SetBit(GPB, 10)
#define DDC114_Stop_Conversion() DrvGPIO_ClrBit(GPB, 10);
#define Xenon_Flash_On() DrvGPIO_SetBit(GPB, 11)
#define Xenon_Flash_Off() DrvGPIO_ClrBit(GPB, 11);
#endif

//#define CONFIG_DIV_Clkout  1/* user def.: output clock output on PB.12 */
#define Measure_phases 2
#define Measure_times 102
//#define DDC114_DEBUG
#define MaestroNano_DEBUG 1
//#define SYS_DEBUG 1
//#define Adjust_Intensity 1
//#define MaestroNano_Fit_Mode 1
#define MaestroNano_OD_Allow_Negative 1
#define MaestroNano_Algo 1
#define Increasing_Sample_Mode 1

extern volatile uint32_t adc_data_ready, redundancy_entry, cur_adc_data_ready;
extern int adc_data[Measure_phases][Measure_times], adc_data1[Measure_phases][Measure_times], adc_data2[Measure_phases][Measure_times], adc_data3[Measure_phases][Measure_times];
extern int most_count, range_start_index, range_end_index; 
extern int adc_temp[4], zero_count, cur_phase;
extern double Pass1_A260_Conc, Start_Dvalid_Xenon_PWM_CNR1, Stop_Dvalid_Xenon_PWM_CNR1, Start_Dvalid_Xenon_PWM_CNR2, Stop_Dvalid_Xenon_PWM_CNR2;

#define Max_Evaluate_Iteration 20
extern Measure_Count;
#endif  //_Measurement_h