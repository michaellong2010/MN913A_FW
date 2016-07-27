#include "DRVSYS.h"
#include "DRVFMC.h"
//#include "i-tracker.h"

/*20140724 added by michael
* read memory data and write into data flash
* read memory-->data flash*/
extern void SysTimerDelay(uint32_t us);
int Programming_Dataflash(uint8_t *src_mem, uint32_t destination_addr, uint32_t bytes) {
	int i32Err, i;
	uint32_t u32Data, u32Data1;

	do {
		//DrvFMC_Erase(destination_addr);

		//words = (!(bytes%sizeof(uint32_t))) ? bytes/sizeof(uint32_t) : (bytes/sizeof(uint32_t)+1);
		i32Err = 0;
		for (i = 0; i < bytes; i += 4) {

			DrvFMC_Read(destination_addr + i, &u32Data);
			if(u32Data != 0xFFFFFFFF)
			{ 
				i32Err = 1;
				break;
			}
			else {
				u32Data = *((uint32_t *)(src_mem + i));

				DrvFMC_Write(destination_addr + i, u32Data);
				//SysTimerDelay(5000);
				DrvFMC_Read(destination_addr + i, &u32Data1);

				if (u32Data != u32Data1) {
					i32Err = 2;
					break;
				}
			}
		}
		//printf("err: %d, index: %d, u32Data: %08x, u32Data1: %08x, bytes: %d\n", i32Err, i, u32Data, u32Data1, bytes);
	}
	while (i32Err > 0);


	if (!i32Err)
		return 0;
	else
		return -1;
}

/*20140731 added by michael
data flash-->read memory*/
int Reading_Dataflash(uint32_t src_addr, uint8_t *destination_mem, uint32_t bytes) {
	int i;
	uint32_t u32Data;

	for (i = 0; i < bytes; i += 4) {

		DrvFMC_Read(src_addr + i, &u32Data);
		*((uint32_t *)(destination_mem + i)) = u32Data;
	}
}