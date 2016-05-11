#include "measurement.h"

volatile uint32_t adc_data_ready = 0, redundancy_entry = 0, cur_adc_data_ready = 0;
double mean[Max_Evaluate_Iteration][4], mean1[Max_Evaluate_Iteration][4];
int adc_data[Measure_phases][Measure_times], adc_data1[Measure_phases][Measure_times], adc_data2[Measure_phases][Measure_times], adc_data3[Measure_phases][Measure_times];
int cur_phase = 1, count2 = 0;
int most_count, range_start_index, range_end_index;
int zero_count = 0, adc_temp[4];
volatile int Measure_Count = Measure_times;

void MaestroNano_Capture(int phase) {
/* pahse 0: blank_init_E or sample_init_E*/
/* pahse 1: blank_E or sample_E*/
  int i = 0;

  //count1 = 0;
  //zero_count = 0;
  //redundancy_entry = 0;
  adc_data_ready = 0;
  cur_adc_data_ready = adc_data_ready;
#ifdef APP_DEBUG
  printf("start adc_data_ready: %d\n", adc_data_ready);
#endif
  /* 20160318 modified by michael */
  DrvTIMER_ResetTicks(TMR2);
  while (adc_data_ready < (Measure_Count+6)) {
    SysTimerDelay(5);
	//if (adc_data_ready==8 && !phase)
	  //break;
	if ( DrvTIMER_GetTicks(TMR2) >= 9 ) {
		printf ( "exit\n" );
		break;
	}
  }

  DDC114_Stop_Conversion();
  Xenon_Flash_Off();
#ifdef APP_DEBUG
  printf("stop adc_data_ready: %d\n", adc_data_ready);
#endif
}

double MaestroNano_Measure(void) {
   int i, j, k, threshold, start_index;
	 double A260 = 0.0;

//capture intensity with Xenon_Flash_On
  for (i = 1; i >= 0; i--) {
     switch (i) {
       case 0:
//	  SysTimerDelay(5000);
      cur_phase = 0;
	  Xenon_Flash_Off();
#ifdef MaestroNano_DEBUG
	      printf("When blank is ready, then press any key\n");
	      printf("measure phase 0: blank_init_E\n");
#endif
//	      getchar();
          DDC114_Start_Conversion();
          //SysTimerDelay(500000);	
          MaestroNano_Capture(i);
		  /*for ( j = 0; j < 102; j++ ) {
		    adc_data[ 0 ][ j ] = adc_data2[ 0 ][ j ] = 3500;
		  }*/
		  //Xenon_Flash_On();
	  break;
       case 1:
	      cur_phase = 1;
#ifdef MaestroNano_DEBUG
	      printf("measure phase 1: blank_E\n");
#endif
		  memset ( adc_data, 0, sizeof ( adc_data ) );
		  memset ( adc_data1, 0, sizeof ( adc_data1 ) );
		  memset ( adc_data2, 0, sizeof ( adc_data2 ) );
		  memset ( adc_data2, 0, sizeof ( adc_data3 ) );
          Xenon_Flash_On();
          //SysTimerDelay(1250); 
		  //SysTimerDelay(31250/2); 
#if (PWM_Count_Method==PWM_Param2) || (PWM_Count_Method==PWM_Param3)
		  while(DrvPWM_GetTimerCounter(DRVPWM_TIMER3)!= 65217 - (65217*1250)/(31250));
#elif (PWM_Count_Method==PWM_Param4)
		  while(DrvPWM_GetTimerCounter(DRVPWM_TIMER3)!=65217/2); 
		  //SysTimerDelay(31250/2); 
#else
          while(DrvPWM_GetTimerCounter(DRVPWM_TIMER3)!= 65217 - (65217*30000)/(31250));
#endif
          DDC114_Start_Conversion();
          //SysTimerDelay(500000);	
          MaestroNano_Capture(i);
		  /*for ( j = 0; j < 102; j++ ) {
		    adc_data[ 1 ][ j ] = adc_data2[ 1 ][ j ] = 500000;
		  }*/
          break;
     }
  }

#ifdef USE_HIST
  quickSort(adc_data1[1], 0, Measure_times-1);
  most_count = 0;
  for (j = 0, k = 0; k < 10; k++)	{
      hist_split[k] = -1;
	  threshold = adc_data1[1][0] + ((adc_data1[1][Measure_times-1] - adc_data1[1][0]) / 10) * (k+1);
	  start_index = j;
	  if (k==9)
	    threshold += 100;
	  while (adc_data1[1][j] <= threshold &&  j < Measure_times)  {
	    hist_split[k]++;
		j++;
	  }

	  if (hist_split[k] >= 0 && hist_split[k] > most_count) {
	    range_start_index = start_index;
		range_end_index = j;
        most_count = hist_split[k] + 1;
	  }
  }
  printf("histogram: %d   %d   %d", range_start_index, range_end_index, most_count);
#else
    range_start_index = 2;
	range_end_index = Measure_Count;
    most_count = range_end_index - range_start_index;
	//most_count = 15;
#endif

#ifdef MaestroNano_DEBUG
  printf("phase 0&1\n");
  mean[count2][0] = mean1[count2][0] = mean[count2][1] = mean1[count2][1] = mean[count2][2] = mean1[count2][2] = mean[count2][3] = mean1[count2][3] = 0;
  //for (k = (Measure_times-10); k < Measure_times; k++) {
  for (k = range_start_index; k < range_end_index; k++) {
     printf("%ld %ld %ld %ld %ld %ld %ld %ld\n", adc_data[0][k], adc_data[1][k], adc_data1[0][k], adc_data1[1][k], adc_data2[0][k], adc_data2[1][k], adc_data3[0][k], adc_data3[1][k]);
//  A280
     mean[count2][0] += adc_data[0][k];
     mean1[count2][0] += adc_data[1][k];
//  A260
     mean[count2][1] += adc_data1[0][k];
     mean1[count2][1] += adc_data1[1][k];
//  A230
     mean[count2][2] += adc_data2[0][k];
     mean1[count2][2] += adc_data2[1][k];
//  A320
     mean[count2][3] += adc_data3[0][k];
     mean1[count2][3] += adc_data3[1][k];
  }
#endif
	A260 = ( mean1[count2][1] - mean[count2][1] ) / ( most_count );
  //most_count_arry[count2] = most_count;
  if (count2==(Max_Evaluate_Iteration-1))
    count2 = 0;
  else
     count2++;
//#endif

//#ifdef 1
#if  ( defined(MaestroNano_DEBUG) && defined(MaestroNano_Fit_Mode) )
  //if (getchar()=='a') {
    for (k = 0; k < count2; k++)  {
       //printf("%lf  %lf  %lf  %lf  %lf  %lf\n", mean[k][0]/(most_count_arry[k]), mean1[k][0]/(most_count_arry[k]), mean[k][1]/(most_count_arry[k]), mean1[k][1]/(most_count_arry[k]), mean[k][2]/(most_count_arry[k]), mean1[k][2]/(most_count_arry[k]));
	   printf("%lf  %lf  %lf  %lf  %lf  %lf %lf %lf\n", mean[k][0]/(most_count), mean1[k][0]/(most_count), mean[k][1]/(most_count), mean1[k][1]/(most_count), mean[k][2]/(most_count), mean1[k][2]/(most_count), mean[k][3]/(most_count), mean1[k][3]/(most_count));
    }
    //getchar();
  //}
#endif
//#endif
  return A260;
}