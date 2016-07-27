/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2010 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "NUC1xx.h"
#include "DrvSYS.h"
#include "DrvUSB.h"
#include "HIDTransferSys.h"
#include "HIDTransfer_API.h"
#include "MN-913A.h"
#include "measurement.h"
#include "DrvFMC.h"
//#define DBG_PRINTF(...)
#define DBG_PRINTF  printf
#define E_FMC_LDROM LDROM



#define USB_VID			0x0416  /* Vendor ID */
#define USB_PID			0x5023  /* Product ID */

#define USB_VID_LO      (USB_VID & 0xFF)
#define USB_VID_HI      ((USB_VID >> 8) & 0xFF)

#define USB_PID_LO      (USB_PID & 0xFF)
#define USB_PID_HI      ((USB_PID >> 8) & 0xFF)

#define HID_DEFAULT_INT_IN_INTERVAL		1
#define HID_IS_SELF_POWERED         	0
#define HID_IS_REMOTE_WAKEUP        	0
#define HID_MAX_POWER               	50  /* The unit is in 2mA. ex: 50 * 2mA = 100mA */

#define LEN_CONFIG_AND_SUBORDINATE (LEN_CONFIG+LEN_INTERFACE+LEN_HID+LEN_ENDPOINT*2)

#define HID_CMD_SIGNATURE   0x43444948

/* HID Transfer Commands */
#define HID_CMD_NONE     0x00
#define HID_CMD_ERASE    0x71
#define HID_CMD_READ     0xD2
#define HID_CMD_WRITE    0xC3
#define HID_CMD_TEST     0xB4

//#define HID_CMD_MN913A_SETTING 0x86
#define PAGE_SIZE		 256

typedef __packed struct {
    uint8_t u8Cmd;
    uint8_t u8Size;
    uint32_t u32Arg1;
    uint32_t u32Arg2;
    uint32_t u32Signature;
    uint32_t u32Checksum;
}CMD_T;

CMD_T gCmd;    

uint8_t recv_cmd = HID_CMD_NONE;
uint8_t g_au8DeviceReport[HID_MAX_PACKET_SIZE_INT_IN];
const uint32_t g_u32DeviceReportSize = sizeof(g_au8DeviceReport) / sizeof(g_au8DeviceReport[0]);

const uint8_t gau8DeviceReportDescriptor[] =
{

 	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
 	0x09, 0x00, // USAGE (0)
 	0xA1, 0x01, // COLLECTION (Application)
 	0x15, 0x00, //     LOGICAL_MINIMUM (0)
 	0x25, 0xFF, //     LOGICAL_MAXIMUM (255)
 	0x19, 0x01, //     USAGE_MINIMUM (1)
 	0x29, 0x08, //     USAGE_MAXIMUM (8) 
 	0x95, HID_MAX_PACKET_SIZE_INT_OUT, //     REPORT_COUNT (8)
 	0x75, 0x08, //     REPORT_SIZE (8)
 	0x81, 0x02, //     INPUT (Data,Var,Abs)
 	0x19, 0x01, //     USAGE_MINIMUM (1)
 	0x29, 0x08, //     USAGE_MAXIMUM (8) 
 	0x91, 0x02, //   OUTPUT (Data,Var,Abs)
 	0xC0        // END_COLLECTION
};

#define HID_DEVICE_REPORT_DESCRIPTOR_SIZE \
	sizeof (gau8DeviceReportDescriptor) / sizeof(gau8DeviceReportDescriptor[0])

const uint32_t gu32DeviceReportDescriptorSize = HID_DEVICE_REPORT_DESCRIPTOR_SIZE;

#define HID_REPORT_DESCRIPTOR_SIZE 		HID_DEVICE_REPORT_DESCRIPTOR_SIZE

extern int8_t imageBegin0, imageEnd;

const uint8_t gau8DeviceDescriptor[LEN_DEVICE] =
{
	LEN_DEVICE,		    		/* bLength 				*/
	DESC_DEVICE,	    		/* bDescriptorType 		*/
	0x10, 0x01,		    		/* bcdUSB 				*/
	0x00,			    		/* bDeviceClass 		*/
	0x00,			    		/* bDeviceSubClass 		*/
	0x00,			    		/* bDeviceProtocol 		*/
	HID_MAX_PACKET_SIZE_CTRL,/* bMaxPacketSize0 	*/
	USB_VID & 0x00FF,			/* Vendor ID 			*/
	(USB_VID & 0xFF00) >> 8, 
	USB_PID & 0x00FF,			/* Product ID 			*/
	(USB_PID & 0xFF00) >> 8, 
	0x00, 0x00,		    		/* bcdDevice 			*/
	0x01,			    		/* iManufacture 		*/
	0x02,			    		/* iProduct				*/
	0x01,			    		/* iSerialNumber 		*/
	0x01			    		/* bNumConfigurations 	*/
};

const uint8_t gau8ConfigDescriptor[] =
{
	LEN_CONFIG,		// bLength
	DESC_CONFIG,	// bDescriptorType
	// wTotalLength
	LEN_CONFIG_AND_SUBORDINATE & 0x00FF,
	(LEN_CONFIG_AND_SUBORDINATE & 0xFF00) >> 8,
	0x01,			// bNumInterfaces
	0x01,			// bConfigurationValue
	0x00,			// iConfiguration
	0xC0,			// bmAttributes
	0x32,			// MaxPower

    
 	LEN_INTERFACE,
 	DESC_INTERFACE,
 	0x00,           // bInterfaceNumber
 	0x00,           // bAlternateSetting
 	0x02,           // bNumEndpoints
 	0xff,           // bInterfaceClass
 	0x00,           // bInterfaceSubClass
 	HID_FUNCTION,   // bInterfaceProtocol
 	0x00,           // iInterface
 

 	LEN_HID,        // bLength
 	DESC_HID,       // bDescriptorType
 	0x10, 0x01,     // bcdHID
 	0x00,           // bCountryCode
 	0x01,           // bNumDescriptor
 	DESC_HID_RPT,   // bDescriptorType
	HID_REPORT_DESCRIPTOR_SIZE & 0x00FF,            // wDescriptorLen
	(HID_REPORT_DESCRIPTOR_SIZE & 0xFF00) >> 8,
	
 	LEN_ENDPOINT,                                   // bLength
 	DESC_ENDPOINT,                                  // bDescriptorType
 	HID_IN_EP_NUM | 0x80,                           // bEndpointAddress
 	EP_BULK,                                         // bmAttributes
 	HID_MAX_PACKET_SIZE_INT_IN & 0x00FF,                // wMaxPacketSize
	(HID_MAX_PACKET_SIZE_INT_IN & 0xFF00) >> 8,         
 	HID_DEFAULT_INT_IN_INTERVAL,                    // bInterval

 	LEN_ENDPOINT,                                   // bLength
 	DESC_ENDPOINT,                                  // bDescriptorType
 	HID_OUT_EP_NUM,                                 // bEndpointAddress
 	EP_BULK,                                         // bmAttributes
	HID_MAX_PACKET_SIZE_INT_IN & 0x00FF,                // wMaxPacketSize
	(HID_MAX_PACKET_SIZE_INT_IN & 0xFF00) >> 8,
 	HID_DEFAULT_INT_IN_INTERVAL                     // bInterval
};

const uint8_t gau8StringLang[] = 
{
	4,				/* bLength                  */
	DESC_STRING,	/* bDescriptorType          */
	0x09, 0x04      /* Language ID: USA(0x0409) */
};

const uint8_t gau8VendorStringDescriptor[] = 
{
	16,             /* bLength          */
	DESC_STRING,    /* bDescriptorType  */
	'N', 0, 
	'u', 0, 
	'v', 0, 
	'o', 0, 
	't', 0, 
	'o', 0, 
	'n', 0
};


const uint8_t gau8ProductStringDescriptor[] = 
{
	26,             /* bLength          */
	DESC_STRING,    /* bDescriptorType  */
	'H', 0, 
	'I', 0, 
	'D', 0, 
	' ', 0, 
	'T', 0, 
	'r', 0, 
	'a', 0, 
	'n', 0, 
	's', 0, 
	'f', 0, 
	'e', 0, 
	'r', 0
};


const uint8_t gau8StringSerial[26] =
{
	26,				// bLength
	DESC_STRING,	// bDescriptorType
	'B', 0, 
	'0', 0, 
	'2', 0, 
	'0', 0, 
	'1', 0, 
	'0', 0, 
	'0', 0, 
	'4', 0, 
	'1', 0, 
	'3', 0, 
	'0', 0, 
	'2', 0
};


static uint8_t  g_u8PageBuff[PAGE_SIZE] = {0};    /* Page buffer to upload/download through HID report */
static uint32_t g_u32BytesInPageBuf = 0;                         /* The bytes of data in g_u8PageBuff */
struct FW_Header updated_fw_header = { 1, {0} , 1, {0}};
static uint8_t  g_u8PageBuff2[PAGE_SIZE] = {0};


int32_t HID_CmdEraseSectors(CMD_T *pCmd)
{
    uint32_t u32StartSector;
    uint32_t u32Sectors;
    uint32_t i;
    
    u32StartSector = pCmd->u32Arg1;
    u32Sectors = pCmd->u32Arg2;

	DBG_PRINTF("Erase command - Sector: %d   Sector Cnt: %d\n", u32StartSector, u32Sectors);
	
	for(i=0;i<u32Sectors;i++)
	{
	    DBG_PRINTF("Erase sector - %d\n",u32StartSector + i);
		/* TODO: To erase the sector of storage */
	
	}
	/* To note the command has been done */
	pCmd->u8Cmd = HID_CMD_NONE;
	
	return 0;
}

int32_t Recv_PRINT_META_DATA ( CMD_T *pCmd )
{
		uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

    DBG_PRINTF("MN913A print meta data - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
	  return 0;
}

int32_t Recv_MN913A_lcd_brightness ( CMD_T *pCmd )
{
	  uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	//DBG_PRINTF("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    DBG_PRINTF("MN913A setting lcd brightness - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
	  return 0;
}

int32_t Recv_Android_systime ( CMD_T *pCmd )
{
	  uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	//DBG_PRINTF("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    DBG_PRINTF("MN913A sync android systime to RTC - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
	  return 0;	
}

int32_t Recv_MN913A_DNA_RESULT ( CMD_T *pCmd )
{
	  uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	//DBG_PRINTF("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    DBG_PRINTF("MN913A dna result - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
	  return 0;
}

int32_t Recv_MN913A_PROTEIN_RESULT ( CMD_T *pCmd )
{
	  uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	//DBG_PRINTF("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    DBG_PRINTF("MN913A protein result - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
	  return 0;
}

int32_t Recv_MN913A_Preference(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	//DBG_PRINTF("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    DBG_PRINTF("Set up MN913A preference - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
    return 0;
}

int32_t Send_MN913A_RAW_DATA(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;
    int32_t i;

    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;
	//DBG_PRINTF("Retrieve i-tracker data - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);

    //if (itracker_dev.buffer_locked)
	  //return;
    if(u32Pages)
    {
        memcpy ( g_u8PageBuff, &adc_data[ 0 ] [ 2 ], 256 );
        g_u32BytesInPageBuf = PAGE_SIZE;
        
        /* The signature word is used as page counter */
        pCmd->u32Signature = 1;
        
	    /* Trigger HID IN */
        //SysTimerDelay(50000);
		DrvUSB_DataIn(HID_IN_EP_NUM, g_u8PageBuff, HID_MAX_PACKET_SIZE_INT_IN);
		g_u32BytesInPageBuf-= HID_MAX_PACKET_SIZE_INT_IN;	
    }

	return 0;
}

int32_t Send_MN913A_STATUS(CMD_T *pCmd)
{
	  uint32_t u32StartPage;
    uint32_t u32Pages;
    int32_t i;

    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

    if(u32Pages)
    {
		//mn913a_status.remain_in_measure = 1;
		//printf ("remain_in_measure: %d\n", mn913a_status.remain_in_measure );
        memcpy ( g_u8PageBuff, ( void * ) &mn913a_status, (unsigned int)sizeof ( struct MN913A_status_type ) );
        g_u32BytesInPageBuf = PAGE_SIZE;
        
        /* The signature word is used as page counter */
        pCmd->u32Signature = 1;
        
	    /* Trigger HID IN */
        //SysTimerDelay(50000);
		DrvUSB_DataIn(HID_IN_EP_NUM, g_u8PageBuff, HID_MAX_PACKET_SIZE_INT_IN);
		g_u32BytesInPageBuf-= HID_MAX_PACKET_SIZE_INT_IN;	
    }
		
	return 0;
}

int32_t Send_RTC_systime(CMD_T *pCmd)
{
	  uint32_t u32StartPage;
    uint32_t u32Pages;
    int32_t i;

    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

    if(u32Pages)
    {
		//mn913a_status.remain_in_measure = 1;
		//printf ("remain_in_measure: %d\n", mn913a_status.remain_in_measure );
        memcpy ( g_u8PageBuff, ( void * ) &mn913a_datetime_data, (unsigned int)sizeof ( struct MN913A_datetime_type ) );
        g_u32BytesInPageBuf = PAGE_SIZE;
        
        /* The signature word is used as page counter */
        pCmd->u32Signature = 1;
        
	    /* Trigger HID IN */
        //SysTimerDelay(50000);
		DrvUSB_DataIn(HID_IN_EP_NUM, g_u8PageBuff, HID_MAX_PACKET_SIZE_INT_IN);
		g_u32BytesInPageBuf-= HID_MAX_PACKET_SIZE_INT_IN;	
    }
		
	return 0;
}

int32_t HID_CmdReadPages(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;
    int32_t i;

    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	DBG_PRINTF("Read command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);

    if(u32Pages)
    {
        /* Update data to page buffer to upload */
		/* TODO: We need to update the page data if got a page read command. (0xFF is used in this sample code) */
        for(i=0;i<PAGE_SIZE;i++)
            g_u8PageBuff[i] = 0xFF;
        g_u32BytesInPageBuf = PAGE_SIZE;
        
        /* The signature word is used as page counter */
        pCmd->u32Signature = 1;
        
	    /* Trigger HID IN */
		DrvUSB_DataIn(HID_IN_EP_NUM, g_u8PageBuff, HID_MAX_PACKET_SIZE_INT_IN);
		g_u32BytesInPageBuf-= HID_MAX_PACKET_SIZE_INT_IN;
//		DrvUSB_DataIn(HID_IN_EP_NUM, g_u8PageBuff, 0);
//		g_u32BytesInPageBuf-= 0;		
    }

	return 0;
}


int32_t HID_CmdWritePages(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	DBG_PRINTF("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;

	return 0;
}




int32_t gi32CmdTestCnt = 0;
int32_t HID_CmdTest(CMD_T *pCmd)
{
    int32_t i;
    uint8_t *pu8;
    
    pu8 = (uint8_t *)pCmd;
    DBG_PRINTF("Get test command #%d (%d bytes)\n", gi32CmdTestCnt++, pCmd->u8Size);
    for(i=0;i<pCmd->u8Size;i++)
    {
        if((i&0xF) == 0)
        {
            DBG_PRINTF("\n");
        }
        DBG_PRINTF(" %02x", pu8[i]);
    }
    
    DBG_PRINTF("\n");
    
    
	/* To note the command has been done */
	pCmd->u8Cmd = HID_CMD_NONE;
	
	return 0;
}


uint32_t CalCheckSum(uint8_t *buf, uint32_t size)
{
    uint32_t sum;
    int32_t i;

    i = 0;
    sum = 0;
    while(size--)
    {
        sum+=buf[i++];
    }

    return sum;

}

int32_t Read_Itrack_Firmware_Header(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;
    int32_t i;
	uint8_t *pBuf = NULL;

    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;
	DBG_PRINTF("Retrieve i-tracker firmware header info - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);

    if(u32Pages)
    {
		/*if (sizeof(updated_fw_header)%PAGE_SIZE)
			Reading_Dataflash(updated_fw_header_base, g_u8PageBuff, PAGE_SIZE * (sizeof(updated_fw_header)/PAGE_SIZE + 1));
		else
			Reading_Dataflash(updated_fw_header_base, g_u8PageBuff, PAGE_SIZE * (sizeof(updated_fw_header)/PAGE_SIZE));*/
		Reading_Dataflash(updated_fw_header_base, g_u8PageBuff, PAGE_SIZE);
		pBuf = (uint8_t *)  &updated_fw_header;
        //memcpy(pBuf, g_u8PageBuff, PAGE_SIZE);
		DrvFMC_Read(updated_fw_header_base + 252, (uint32_t *) (g_u8PageBuff + 252));
        g_u32BytesInPageBuf = PAGE_SIZE;
        
        /* The signature word is used as page counter */
        pCmd->u32Signature = 1;
        
	    /* Trigger HID IN */
        //SysTimerDelay(50000);
		DrvUSB_DataIn(HID_IN_EP_NUM, g_u8PageBuff, HID_MAX_PACKET_SIZE_INT_IN);
		g_u32BytesInPageBuf-= HID_MAX_PACKET_SIZE_INT_IN;	
    }

	return 0;
}

/*200160705   jan
data flash-->read memory*/
/*int Reading_Dataflash(uint32_t src_addr, uint8_t *destination_mem, uint32_t bytes) {
	int i;
	uint32_t u32Data;

	for (i = 0; i < bytes; i += 4) {

		DrvFMC_Read(src_addr + i, &u32Data);
		*((uint32_t *)(destination_mem + i)) = u32Data;
	}
} */

int32_t Upgrade_Itrack_Firmware(CMD_T *pCmd)
{
    uint32_t u32StartPage;
    uint32_t u32Pages;
    
    u32StartPage = pCmd->u32Arg1;
    u32Pages     = pCmd->u32Arg2;

	//DBG_PRINTF("Write command - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    //DBG_PRINTF("Upgrading i-tracker firmware - Start page: %d    Pages Numbers: %d\n", u32StartPage, u32Pages);
    g_u32BytesInPageBuf = 0;
    
    /* The signature is used to page counter */
    pCmd->u32Signature = 0;
    return 0;
}

int32_t ProcessCommand(uint8_t *pu8Buffer, uint32_t u32BufferLen)
{
	uint32_t u32sum;
	int i = 0;

	
    DrvUSB_memcpy((uint8_t *)&gCmd, pu8Buffer, u32BufferLen);
/*20130315 added by michael*/    
//for debug tablet usb command send
//printf("receive command size: %d\n",  u32BufferLen);
//printf("data byte: ");
/*for (i = 0; i <  u32BufferLen; i++) {
   printf("%02x", pu8Buffer[i]);
}
printf("\n");*/
    /* Check size */
    if((gCmd.u8Size > sizeof(gCmd)) || (gCmd.u8Size > u32BufferLen))    
        return -1;

    /* Check signature */
    if(gCmd.u32Signature != HID_CMD_SIGNATURE)
        return -1;
        
    /* Calculate checksum & check it*/        
    u32sum = CalCheckSum((uint8_t *)&gCmd, gCmd.u8Size);
    if(u32sum != gCmd.u32Checksum)
        return -1;
    
	switch(gCmd.u8Cmd)
	{
		case HID_CMD_ERASE:
		{
			HID_CmdEraseSectors(&gCmd);
			break;
		}		
		case HID_CMD_READ:
		{
			HID_CmdReadPages(&gCmd);
			break;
		}		
		case HID_CMD_WRITE:
		{
			HID_CmdWritePages(&gCmd);
			break;		
		}
		case HID_CMD_TEST:
		{
		    HID_CmdTest(&gCmd);
		    break;
		}
		case HID_CMD_MN913A_SETTING:  //OUT
			Recv_MN913A_Preference(&gCmd);
            break;
		case HID_CMD_MN913A_MEASURE:
			recv_cmd = HID_CMD_MN913A_MEASURE;
			mn913a_status.remain_in_measure = 1;
		    break;
		case HID_CMD_MN913A_RAW_DATA:
			if ( recv_cmd == HID_CMD_MN913A_MEASURE ) {
				printf ( "still in measuring!\n" );
			}
			else {
				Send_MN913A_RAW_DATA ( &gCmd );
			}
			break;
		case HID_CMD_MN913A_STATUS:
			Send_MN913A_STATUS ( &gCmd );
			break;
		case HID_CMD_PRINT_DNA_RESULT:
			Recv_MN913A_DNA_RESULT ( &gCmd );
			break;
		case HID_CMD_PRINT_PROTEIN_RESULT:
			Recv_MN913A_PROTEIN_RESULT ( &gCmd );
			break;
		case HID_CMD_GET_TIME: //IN
			Get_Maestro_RTC ( &temp_date1, i);
			mn913a_datetime_data.year = temp_date1.PackDate.Year;
	    mn913a_datetime_data.month = temp_date1.PackDate.Month;
	    mn913a_datetime_data.dayofmonth = temp_date1.PackDate.Day;
	    mn913a_datetime_data.hour = temp_date1.PackDate.Hour;
	    mn913a_datetime_data.minute = temp_date1.PackDate.Minute;
	    mn913a_datetime_data.second = temp_date1.PackDate.Second;
			Send_RTC_systime ( &gCmd );
			break;
    case HID_CMD_SET_TIME: //OUT
			Recv_Android_systime ( &gCmd );
			break;
		case HID_CMD_SET_LCD_BRIGHTNESS: //OUT
			Recv_MN913A_lcd_brightness ( &gCmd );
			break;
		case HID_CMD_PRINT_META_DATA: //OUT
			Recv_PRINT_META_DATA ( &gCmd );
			break;
		case HID_CMD_MN913_FW_UPGRADE:  //OUT	 //20160705   jan
			//	DrvSYS_UnlockProtectedReg();
			//	printf ( "************ HID_CMD_MN913_FW_UPGRADE\n");  //version name: 1.0.20160711
			printf ( "Server HID_CMD_MN913_FW_UPGRADE\n");  
			DrvSYS_UnlockKeyAddr();
			DrvFMC_EnableISP(1);
			Upgrade_Itrack_Firmware(&gCmd);
			break;

		case HID_CMD_MN913_FW_HEADER:  //IN		 //20160705   jan
			//   printf ( "##########  HID_CMD_MN913_FW_HEADER\n");//version name: 1.0.20160711			  
			//		DrvSYS_UnlockProtectedReg();
			printf ( "Server HID_CMD_MN913_FW_HEADER\n");  
			DrvSYS_UnlockKeyAddr();
			DrvFMC_EnableISP(1);
			Read_Itrack_Firmware_Header(&gCmd);

			break;
		case HID_CMD_PRINTER_POWER_ON:
			recv_cmd = HID_CMD_PRINTER_POWER_ON;
			break;
    case HID_CMD_PRINTER_POWER_OFF:
			recv_cmd = HID_CMD_PRINTER_POWER_OFF;
			break;
		default:
			return -1;
	}	
	
	return 0;
}


void HID_SetOutReport(uint8_t *pu8EpBuf, uint32_t u32Size)
{
    uint8_t  u8Cmd;
    uint32_t u32StartPage;
    uint32_t u32Pages;
    uint32_t u32PageCnt;
	uint8_t *pBuf = NULL;

    /* Get command information */
    u8Cmd        = gCmd.u8Cmd;
    u32StartPage = gCmd.u32Arg1;
    u32Pages     = gCmd.u32Arg2;
    u32PageCnt   = gCmd.u32Signature; /* The signature word is used to count pages */
    

    /* Check if it is in the data phase of write command */
    if((u8Cmd == HID_CMD_WRITE) &&  (u32PageCnt < u32Pages))
    {
        /* Process the data phase of write command */


        /* Get data from HID OUT */
		DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
		g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;

        /* The HOST must make sure the data is PAGE_SIZE alignment */
		if(g_u32BytesInPageBuf >= PAGE_SIZE)
		{
		    DBG_PRINTF("Writing page %d\n", u32StartPage + u32PageCnt);
			/* TODO: We should program received data to storage here */
			u32PageCnt++;
		
		    /* Write command complete! */
			if(u32PageCnt >= u32Pages)
			{
				u8Cmd = HID_CMD_NONE;	
		        
		        DBG_PRINTF("Write command complete.\n");
		    }

  			g_u32BytesInPageBuf = 0;

		}

		/* Update command status */
		gCmd.u8Cmd        = u8Cmd;
		gCmd.u32Signature = u32PageCnt;
    }
		else
		if ((u8Cmd == HID_CMD_MN913A_SETTING) &&  (u32PageCnt < u32Pages)) {
			/* Get data from HID OUT */
			DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
			g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;

			pBuf = (uint8_t *)  &mn913a_preference;
			if (g_u32BytesInPageBuf >= PAGE_SIZE) {
				if ((sizeof(mn913a_preference) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
					memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
				}
				else {
					memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, sizeof(mn913a_preference) - u32PageCnt*PAGE_SIZE);
				}

				u32PageCnt++;		
				/* Setup preference command complete! */
				if (u32PageCnt >= u32Pages)
				{
					u8Cmd = HID_CMD_NONE;	

					DBG_PRINTF("Setup preference command complete.\n");
					recv_cmd = HID_CMD_MN913A_SETTING;
					mn913a_status.max_voltage_level = mn913a_preference.max_voltage_level;
					mn913a_status.min_voltage_level = mn913a_preference.min_voltage_level;
					mn913a_status.has_calibration = mn913a_preference.has_calibration;
					mn913a_status.max_voltage_intensity = mn913a_preference.max_voltage_intensity;
					mn913a_status.min_voltage_intensity = mn913a_preference.min_voltage_intensity;
				}
				g_u32BytesInPageBuf = 0;
			}
			/* Update command status */
			gCmd.u8Cmd        = u8Cmd;
			gCmd.u32Signature = u32PageCnt;
		}
    else
			  if ((u8Cmd == HID_CMD_PRINT_DNA_RESULT) &&  (u32PageCnt < u32Pages)) {
			DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
			g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;

			pBuf = (uint8_t *)  &mn913a_dna_result_data;
			if (g_u32BytesInPageBuf >= PAGE_SIZE) {
				if ((sizeof(mn913a_dna_result_data) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
					memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
				}
				else {
					memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, sizeof(mn913a_dna_result_data) - u32PageCnt*PAGE_SIZE);
				}

				u32PageCnt++;		
				/* Setup preference command complete! */
				if (u32PageCnt >= u32Pages)
				{
					u8Cmd = HID_CMD_NONE;	

					DBG_PRINTF("Transfer dna result command complete %d %d.\n", mn913a_dna_result_data.count, mn913a_dna_result_data.type);
					recv_cmd = HID_CMD_PRINT_DNA_RESULT;
				}
				g_u32BytesInPageBuf = 0;
			}
			/* Update command status */
			gCmd.u8Cmd        = u8Cmd;
			gCmd.u32Signature = u32PageCnt;
				}
				else
				   if ((u8Cmd == HID_CMD_PRINT_PROTEIN_RESULT) &&  (u32PageCnt < u32Pages)) {
			DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
			g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;

			pBuf = (uint8_t *)  &mn913a_protein_result_data;
			if (g_u32BytesInPageBuf >= PAGE_SIZE) {
				if ((sizeof(mn913a_protein_result_data) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
					memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
				}
				else {
					memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, sizeof(mn913a_protein_result_data) - u32PageCnt*PAGE_SIZE);
				}

				u32PageCnt++;
				/* Setup preference command complete! */
				if (u32PageCnt >= u32Pages)
				{
					u8Cmd = HID_CMD_NONE;	

					DBG_PRINTF("Transfer protein result command complete %d.\n", mn913a_protein_result_data.count);
					recv_cmd = HID_CMD_PRINT_PROTEIN_RESULT;
				}
				g_u32BytesInPageBuf = 0;
			}
			/* Update command status */
			gCmd.u8Cmd        = u8Cmd;
			gCmd.u32Signature = u32PageCnt;
					 }
				else
					if ((u8Cmd == HID_CMD_SET_TIME) &&  (u32PageCnt < u32Pages)) {
						DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
						g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;
						pBuf = (uint8_t *)  &mn913a_datetime_data;
						if (g_u32BytesInPageBuf >= PAGE_SIZE) {
							if ((sizeof(mn913a_datetime_data) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
								memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
							}
							else {
								memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, sizeof(mn913a_datetime_data) - u32PageCnt*PAGE_SIZE);
							}

							u32PageCnt++;
							/* Setup preference command complete! */
							if (u32PageCnt >= u32Pages)
							{
								u8Cmd = HID_CMD_NONE;	

								//DBG_PRINTF("Transfer android systime command complete %d.\n", mn913a_protein_result_data.count);
								printf("Y/M/D H:M:S=%d/%d/%d %d:%d:%d", mn913a_datetime_data.year, mn913a_datetime_data.month, mn913a_datetime_data.dayofmonth, mn913a_datetime_data.hour, mn913a_datetime_data.minute, mn913a_datetime_data.second );
								recv_cmd = HID_CMD_SET_TIME;
							}
							g_u32BytesInPageBuf = 0;
						}

						/* Update command status */
						gCmd.u8Cmd        = u8Cmd;
						gCmd.u32Signature = u32PageCnt;
					}
					else
						if ( ( u8Cmd == HID_CMD_SET_LCD_BRIGHTNESS ) &&  ( u32PageCnt < u32Pages ) ) {
							DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
							g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;
							pBuf = (uint8_t *)  &mn913a_lcd_brightness;
							if (g_u32BytesInPageBuf >= PAGE_SIZE) {
								if ((sizeof ( mn913a_lcd_brightness ) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
									memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
								}
								else {
									memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, sizeof ( mn913a_lcd_brightness ) - u32PageCnt*PAGE_SIZE);
								}

								u32PageCnt++;
								/* Setup preference command complete! */
								if (u32PageCnt >= u32Pages)
								{
									u8Cmd = HID_CMD_NONE;

									DBG_PRINTF("Setup lcd_brightness: %d command complete.\n", mn913a_lcd_brightness);
									recv_cmd = HID_CMD_SET_LCD_BRIGHTNESS;
								}
								g_u32BytesInPageBuf = 0;
							}

							/* Update command status */
							gCmd.u8Cmd        = u8Cmd;
							gCmd.u32Signature = u32PageCnt;
						}
												 else
	   if ((u8Cmd == HID_CMD_MN913_FW_UPGRADE) &&  (u32PageCnt < u32Pages)) {
		   DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
		   g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;

		   //pBuf = (uint8_t *)  &updated_fw_header;

		   if (g_u32BytesInPageBuf >= PAGE_SIZE) {
			   memcpy(g_u8PageBuff2, g_u8PageBuff, PAGE_SIZE);
			   if (u32PageCnt==0) {
				   /*if ((sizeof(updated_fw_header) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
					   memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
				   }
				   else 
					   if ((sizeof(updated_fw_header) - u32PageCnt*PAGE_SIZE) > 0)
						   memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, sizeof(updated_fw_header) - u32PageCnt*PAGE_SIZE);*/
				   
				   //DrvFMC_Erase(updated_fw_header_base);
				   //Programming_Dataflash(g_u8PageBuff, updated_fw_header_base, sizeof(updated_fw_header));
				   if (!u32StartPage) {
					   //printf("erase addr: %08x\n", updated_fw_header_base);
					   DrvFMC_Erase(updated_fw_header_base);
					   Programming_Dataflash(g_u8PageBuff2, updated_fw_header_base, PAGE_SIZE);
				   }
				   else
					   if (u32StartPage > 0) {
						   if (u32StartPage%2) {
							   //printf("erase addr: %08x\n", updated_fw_bin_base + (u32StartPage-1) * PAGE_SIZE);
							   DrvFMC_Erase(updated_fw_bin_base + (u32StartPage-1) * PAGE_SIZE);
							   Programming_Dataflash(g_u8PageBuff2, updated_fw_bin_base + (u32StartPage-1) * PAGE_SIZE, PAGE_SIZE);
						   }
						   else {
							   Programming_Dataflash(g_u8PageBuff2, updated_fw_bin_base + (u32StartPage-1) * PAGE_SIZE, PAGE_SIZE);
						   }
					   }
			   }
			   else {
			   }

               //DrvFMC_Erase(updated_fw_bin_base + u32StartPage * PAGE_SIZE);
			   //Programming_Dataflash(g_u8PageBuff, updated_fw_bin_base + u32StartPage * PAGE_SIZE, PAGE_SIZE);
			   u32PageCnt++;		
			   /* Upgrade firmware command complete! */
			   if (u32PageCnt >= u32Pages)
			   {
				   u8Cmd = HID_CMD_NONE;	

				   DrvFMC_EnableISP(1);
				   if (!u32StartPage) {
					   DBG_PRINTF("Upgrade firmware command complete.\n");
				       DrvFMC_BootSelect(E_FMC_LDROM);
					   DrvSYS_ResetCPU();
				   }
				   DrvSYS_LockKeyAddr();
				  // DrvSYS_LockProtectedReg();
			   }
			   g_u32BytesInPageBuf = 0;
		   }
		   /* Update command status */
		   gCmd.u8Cmd        = u8Cmd;
		   gCmd.u32Signature = u32PageCnt;
	   }
						else
							if ( ( u8Cmd == HID_CMD_PRINT_META_DATA ) &&  ( u32PageCnt < u32Pages ) ) {
								DrvUSB_memcpy(&g_u8PageBuff[g_u32BytesInPageBuf], pu8EpBuf, HID_MAX_PACKET_SIZE_INT_IN);
								g_u32BytesInPageBuf += HID_MAX_PACKET_SIZE_INT_IN;
								pBuf = (uint8_t *)  &meta_print_data;
								if (g_u32BytesInPageBuf >= PAGE_SIZE) {
									if ((sizeof ( meta_print_data ) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
										memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
									}
									else {
										memcpy(pBuf+u32PageCnt*PAGE_SIZE, g_u8PageBuff, sizeof ( meta_print_data ) - u32PageCnt*PAGE_SIZE);
									}
									u32PageCnt++;
									/* Setup preference command complete! */
									if (u32PageCnt >= u32Pages)
									{
										u8Cmd = HID_CMD_NONE;
										DBG_PRINTF("Transfer meta print data command complete %d.\n", meta_print_data.type_id );
										recv_cmd = HID_CMD_PRINT_META_DATA;
									}
									g_u32BytesInPageBuf = 0;
								}
								/* Update command status */
								gCmd.u8Cmd        = u8Cmd;
								gCmd.u32Signature = u32PageCnt;
							}
							else
    {
        /* Check and process the command packet */
        if(ProcessCommand(pu8EpBuf, u32Size))
        {
            DBG_PRINTF("Unknown HID command!\n");
        }        
    }
}

void HID_GetInReport(uint8_t *buf)
{
    uint32_t u32StartPage;
    uint32_t u32TotalPages;
    uint32_t u32PageCnt;
    uint8_t u8Cmd;
	int32_t i;
	uint8_t *pBuf = NULL, base_page = 0;
    
    u8Cmd        = gCmd.u8Cmd;
    u32StartPage = gCmd.u32Arg1;
    u32TotalPages= gCmd.u32Arg2;
    u32PageCnt   = gCmd.u32Signature;

    /* Check if it is in data phase of read command */
	if(u8Cmd == HID_CMD_READ)
	{
	    /* Process the data phase of read command */
        if((u32PageCnt >= u32TotalPages) && (g_u32BytesInPageBuf == 0))
        {
    		/* The data transfer is complete. */
            u8Cmd = HID_CMD_NONE;
            DBG_PRINTF("Read command complete!\n");
        }    
	    else
	    {
	        if(g_u32BytesInPageBuf == 0)
	        {
	            /* The previous page has sent out. Read new page to page buffer */
				/* TODO: We should update new page data here. (0xFF is used in this sample code) */
				DBG_PRINTF("Reading page %d\n", u32StartPage + u32PageCnt);
		        for(i=0;i<PAGE_SIZE;i++)
		            g_u8PageBuff[i] = 0xFF;
					
	            g_u32BytesInPageBuf = PAGE_SIZE;
	        
	            /* Update the page counter */
	            u32PageCnt++;
	        }
	    
            /* Prepare the data for next HID IN transfer */	    
    		DrvUSB_DataIn(HID_IN_EP_NUM, &g_u8PageBuff[PAGE_SIZE - g_u32BytesInPageBuf], HID_MAX_PACKET_SIZE_INT_IN);
    		g_u32BytesInPageBuf -= HID_MAX_PACKET_SIZE_INT_IN;
		}
	}
		else
			if ( u8Cmd == HID_CMD_MN913A_STATUS ) {
				if ( ( u32PageCnt >= u32TotalPages ) && ( g_u32BytesInPageBuf == 0 ) )
				{
					/* The data transfer is complete. Reset coordinate buffer */
					u8Cmd = HID_CMD_NONE;
					//DBG_PRINTF("MN913A transfer status complete!\n");
				}
				else {
					DrvUSB_DataIn(HID_IN_EP_NUM, &g_u8PageBuff[PAGE_SIZE - g_u32BytesInPageBuf], HID_MAX_PACKET_SIZE_INT_IN);
					g_u32BytesInPageBuf -= HID_MAX_PACKET_SIZE_INT_IN;
				}
			}
	else
		/*adc_data[0][102] align page 0~1
		  adc_data[1][102] align page 2~3
		  adc_data1[0][102] align page 4~5
		  adc_data1[1][102] align page 6~7
		  adc_data2[0][102] align page 8~9
		  adc_data2[1][102] align page 10~11
		  adc_data3[0][102] align page 12~13
		  adc_data3[1][102] align page 14~15
		*/
		if ( u8Cmd == HID_CMD_MN913A_RAW_DATA ) {
			if ( ( u32PageCnt >= u32TotalPages ) && ( g_u32BytesInPageBuf == 0 ) )
			{
				/* The data transfer is complete. Reset coordinate buffer */
				u8Cmd = HID_CMD_NONE;
				DBG_PRINTF("MN913A transfer raw data complete!\n");
			}
			else {
				if ( g_u32BytesInPageBuf == 0 ) {
					if ( u32PageCnt >= 0 && u32PageCnt < 2 ) {
						pBuf = ( uint8_t * )  &adc_data[0][2];
						base_page = 0;
					}
					else
						if ( u32PageCnt >= 2 && u32PageCnt < 4 ) {
							pBuf = ( uint8_t * )  &adc_data[1][2];
							base_page = 2;
						}
						else
							if ( u32PageCnt >= 4 && u32PageCnt < 6 ) {
								pBuf = ( uint8_t * )  &adc_data1[0][2];
								base_page = 4;
							}
							else
								if ( u32PageCnt >= 6 && u32PageCnt < 8 ) {
									pBuf = ( uint8_t * )  &adc_data1[1][2];
									base_page = 6;
								}
								else
									if ( u32PageCnt >= 8 && u32PageCnt < 10 ) {
										pBuf = ( uint8_t * )  &adc_data2[0][2];
										base_page = 8;
									}
									else
										if ( u32PageCnt >= 10 && u32PageCnt < 12 ) {
											pBuf = ( uint8_t * )  &adc_data2[1][2];
											base_page = 10;
										}
										else
											if ( u32PageCnt >= 12 && u32PageCnt < 14 ) {
												pBuf = ( uint8_t * )  &adc_data3[0][2];
												base_page = 12;
											}
											else
												if ( u32PageCnt >= 14 && u32PageCnt < 16 ) {
													pBuf = ( uint8_t * )  &adc_data3[1][2];
													base_page = 14;
												}

					if ( ( 400 - ( u32PageCnt - base_page ) * PAGE_SIZE ) >= PAGE_SIZE )
						memcpy( g_u8PageBuff, pBuf + ( u32PageCnt - base_page ) * PAGE_SIZE, PAGE_SIZE );
					else {
						memcpy( g_u8PageBuff, pBuf + ( u32PageCnt - base_page ) * PAGE_SIZE, ( 400 - ( u32PageCnt - base_page ) * PAGE_SIZE ) );
						memset( g_u8PageBuff + ( 400 - ( u32PageCnt - base_page ) * PAGE_SIZE ), 0,  PAGE_SIZE - ( 400 - ( u32PageCnt - base_page ) * PAGE_SIZE ) );
					}
					g_u32BytesInPageBuf = PAGE_SIZE;
					u32PageCnt++;
				}
				DrvUSB_DataIn(HID_IN_EP_NUM, &g_u8PageBuff[PAGE_SIZE - g_u32BytesInPageBuf], HID_MAX_PACKET_SIZE_INT_IN);
				g_u32BytesInPageBuf -= HID_MAX_PACKET_SIZE_INT_IN;
			}
		}
		else
			if ( u8Cmd == HID_CMD_GET_TIME ) {
				if ( ( u32PageCnt >= u32TotalPages ) && ( g_u32BytesInPageBuf == 0 ) )
				{
					/* The data transfer is complete. Reset coordinate buffer */
					u8Cmd = HID_CMD_NONE;
					DBG_PRINTF("MN913A read RTC time completed!\n");
				}
				else {
					/*if ( g_u32BytesInPageBuf == 0 ) {
						g_u32BytesInPageBuf = PAGE_SIZE;
						u32PageCnt++;
					}*/
					DrvUSB_DataIn(HID_IN_EP_NUM, &g_u8PageBuff[PAGE_SIZE - g_u32BytesInPageBuf], HID_MAX_PACKET_SIZE_INT_IN);
					g_u32BytesInPageBuf -= HID_MAX_PACKET_SIZE_INT_IN;
				}
			}

        else
					if (u8Cmd == HID_CMD_MN913_FW_HEADER) {
				if ((u32PageCnt >= u32TotalPages) && (g_u32BytesInPageBuf == 0)) {
					u8Cmd = HID_CMD_NONE;
				//	DrvFMC_DisableISP();
				    DrvFMC_EnableISP(1);
					DrvSYS_LockKeyAddr();
				//	DrvSYS_LockProtectedReg();
					DBG_PRINTF("####  Read firmware header command complete.\n");
				}
				else {
					if (g_u32BytesInPageBuf == 0) {
						pBuf = (uint8_t *)  &updated_fw_header;
						if ((sizeof(updated_fw_header) - u32PageCnt*PAGE_SIZE) >= PAGE_SIZE) {
							Reading_Dataflash(updated_fw_header_base + u32PageCnt * PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
							memcpy(pBuf + u32PageCnt * PAGE_SIZE, g_u8PageBuff, PAGE_SIZE);
						}
						else 
							if ((sizeof(updated_fw_header) - u32PageCnt*PAGE_SIZE) >= 0) {
								Reading_Dataflash(updated_fw_header_base + u32PageCnt * PAGE_SIZE, g_u8PageBuff, (sizeof(updated_fw_header) - u32PageCnt * PAGE_SIZE));
							    memcpy(pBuf + u32PageCnt * PAGE_SIZE, g_u8PageBuff, (sizeof(updated_fw_header) - u32PageCnt * PAGE_SIZE));
							    memset(g_u8PageBuff + (sizeof(updated_fw_header) - u32PageCnt * PAGE_SIZE), 0,  PAGE_SIZE - (sizeof(updated_fw_header) - u32PageCnt * PAGE_SIZE));
							}
						g_u32BytesInPageBuf = PAGE_SIZE;
						u32PageCnt++;
					}
					DrvUSB_DataIn(HID_IN_EP_NUM, &g_u8PageBuff[PAGE_SIZE - g_u32BytesInPageBuf], HID_MAX_PACKET_SIZE_INT_IN);
					g_u32BytesInPageBuf -= HID_MAX_PACKET_SIZE_INT_IN;
				}
			}
	
	gCmd.u8Cmd        = u8Cmd;
	gCmd.u32Signature = u32PageCnt; 
	
}

void Delay(uint32_t delayCnt)
{
    while(delayCnt--)
    {
        __NOP();
        __NOP();
    }
}

void HID_MainProcess(void)
{
	E_DRVUSB_STATE eUsbState;
    extern void Delay(uint32_t delayCnt);

    DrvUSB_Open((void *)DrvUSB_DispatchEvent);
    
    HID_Open((void *)HID_GetInReport, (void *)HID_SetOutReport); 
        
    eUsbState = DrvUSB_GetUsbState();
    
    if (eUsbState >= eDRVUSB_ATTACHED)
    {
        _DRVUSB_ENABLE_SE0();
        Delay(1000);
        _DRVUSB_DISABLE_SE0();
		//printf("initial\n");
    }
    
	
	/* Disable USB-related interrupts */
	_DRVUSB_ENABLE_MISC_INT(0);

	/* Enable float-detection interrupt */
	_DRVUSB_ENABLE_FLD_INT();
	
    /* Enable USB relative interrupt */    
	_DRVUSB_ENABLE_MISC_INT(IEF_WAKEUP | IEF_WAKEUPEN | IEF_FLD | IEF_USB | IEF_BUS);
 
    while(1)
    {
        eUsbState = DrvUSB_GetUsbState();
        
        if ( eUsbState == eDRVUSB_DETACHED )
        {
            DBG_PRINTF("USB Detached.\n");
            /* Just waiting for USB attach */
            while (eUsbState == eDRVUSB_DETACHED)
            {
                eUsbState = DrvUSB_GetUsbState();
            }
        }
    }
}



