/*
 * Empty C++ Application
 */
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "dpti/dpti.h"
#include "ZmodDAC1411/zmoddac1411.h"

XStatus DemoInitialize();
void DemoRun();
#define addrZmodI2C	0x30

#define NODATA -1
#define WRITEWAVE_OP 1
#define START_OP 2
#define STOP_OP 3
#define SETFREQ_OP 4
#define SETGAIN_OP 5


ZMODDAC1411 ZmodDAC(XPAR_ZMODDAC_0_AXI_ZMODDAC1411_V1_0_0_BASEADDR, XPAR_AXIDMA_0_BASEADDR, XPAR_AXI_IIC_0_BASEADDR, addrZmodI2C, XPAR_MICROBLAZE_0_AXI_INTC_ZMODDAC_0_AXI_DMA_0_MM2S_INTROUT_INTR);

int main()
{

    if(DemoInitialize()!=XST_SUCCESS){
    	xil_printf("Error initializing demo\r\n");
    	cleanup_platform();
    	return -1;
    }
    DemoRun();

    cleanup_platform();
    return 0;
}

XStatus DemoInitialize(){
	XStatus Status;
    init_platform();


    Status = initDPTI(XPAR_AXIDMA_1_DEVICE_ID);
    if(Status != XST_SUCCESS){
    	xil_printf("Error initializing DPTI\r\n");
    	return XST_FAILURE;
    }

    DPTI_RequestHeader();//Initial DMA request for 8 byte header

    xil_printf("USB104A7 ZmodDAC Demo Initialized\r\n");
    return XST_SUCCESS;
}

void testDAC() {
    uint32_t *buf = (uint32_t *)ZmodDAC.allocChannelsBuffer(0x400 << 2);
    if(buf==NULL){

    	return;
    }
    // Prepare buffer
    for (int i = 0; i < 0x400; i++) {
        buf[i] = ZmodDAC.arrangeChannelData(0, i << 3); // channel A
    }

    // Send data to DAC and start the instrument
    ZmodDAC.setData(buf, 0x400);
    ZmodDAC.start();
}

void DemoRun(){
	int operation=0;
	int nSamples;
	uint32_t param;
	uint32_t channel;
	int szRecvBuffer=0;
	int16_t* recvBuffer=NULL;
	uint32_t* channelBuffer=NULL;

	while(1){
		if((operation = DPTI_GetOpcode())!=NODATA){
			//ZmodDAC Channel is specified in the 16th bit of the opcode
			if(operation>>16 == 1){
				channel = 0;//Channel 1
			}
			else if(operation>>16 == 2){
				channel = 1;//Channel 2
			}
			else{
				xil_printf("ZmodDAC Channel not specified\r\n");
			}

			switch(operation&0xF){
			case WRITEWAVE_OP:
				//Get the length in bytes parameter from PC
				if((param = DPTI_GetParameter())==0){
					break;
				}
				//Each sample is 2 bytes.
				nSamples=param/2;
				xil_printf("Preparing to read %d samples\r\n", nSamples);

				//If the new length is longer than a previous length, reallocate memory
				if(recvBuffer!=NULL && szRecvBuffer<nSamples*2){
					//recvBuffer is too small to receive all of the data
					free(recvBuffer);
					recvBuffer=NULL;
					if(channelBuffer!=NULL)realloc(channelBuffer, nSamples*2);//Realloc channelbuffer to preserve old data
				}
				//Allocate memory to DPTI receive buffer
				if(recvBuffer==NULL){
					recvBuffer=(int16_t*)malloc(nSamples*2);
					szRecvBuffer=nSamples*2;
					//Check to see if it worked
					if(recvBuffer==NULL){
						xil_printf("Couldn't allocate memory to recvBuffer! Add memory to heap in lscript.ld.\r\n");
						xil_printf("Exiting...\r\n");
						return;
					}
				}

				//Get waveform data from PC
				if(DPTI_ReceiveData(nSamples*2, recvBuffer)==XST_FAILURE){
					//Failed to get data (timeout)
					xil_printf("Failed to receive waveform data\r\n");
					break;
				}

				//If channelBuffer has not been allocated, allocate memory to the buffer.
				if(channelBuffer==NULL){
					//Each sample received is 2 bytes. This will allocate nSamples*4 bytes, or 2 bytes per sample per channel.
					channelBuffer = (uint32_t*)calloc(nSamples, sizeof(uint32_t));
					//Check to see if it worked
					if(channelBuffer==NULL){
						xil_printf("Couldn't allocate memory to channelBuffer! Add memory to heap in the lscript.ld.\r\n");
						xil_printf("Exiting...\r\n");
						return;
					}
				}
				for (int i = 0; i < nSamples; i++) {
					//If channel 1 (channel = 0), top 16 bits get set to 0 then written.
					//If channel 2 (channel = 1), bottom 16 bits get set to 0 then written.
					//(channel<<4) = 16 if channel=1, so 0xFFFF<<16 = 0xFFFF0000
					channelBuffer[i] = (channelBuffer[i] & (0x0000FFFF << (channel<<4))) |
							ZmodDAC.arrangeSignedChannelData(channel, recvBuffer[i]);
				}
				//Set the data in DMA
				ZmodDAC.setData(channelBuffer, nSamples);
				xil_printf("Successfully wrote waveform to channel %d\r\n", channel+1);
				break;
			case START_OP:
				ZmodDAC.start();
				xil_printf("Started channel %d\r\n", channel+1);
				break;
			case STOP_OP:
				ZmodDAC.stop();
				xil_printf("Stopped channel %d\r\n", channel+1);
				break;
			case SETFREQ_OP:
				//Get divider parameter from PC
				param = DPTI_GetParameter();
				if(param==0 || param>0x3FFF){
					xil_printf("Error: Invalid divider value: %d, expected [1-0x3FFF]\r\n", param);
					break;
				}

				ZmodDAC.setOutputSampleFrequencyDivider(param);
				xil_printf("Set output frequency divider to %d\r\n", param);
				break;
			case SETGAIN_OP:
				//Get gain parameter from PC
				param = DPTI_GetParameter();
				if(param > 1){
					xil_printf("Error: Invalid gain value %d, expected [0-1]\r\n", param);
				}

				ZmodDAC.setGain(channel, param);
				xil_printf("Set channel %d gain %s\r\n", channel+1, (param==0) ? "low":"high");
			default:
				xil_printf("Unknown Operation %d\r\n", operation);
			}

			DPTI_RequestHeader();//Request a new header
		}
	}

}


