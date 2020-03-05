#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#undef WIN32
#endif
#include "dpcdecl.h"
#include "dmgr.h"
#include "dpti.h"


//Op codes
#define WRITEWAVE_OP 1
#define START_OP 2
#define STOP_OP 3
#define SET_FREQ_DIV_OP 4
#define SET_GAIN_OP 5

uint8_t channel = 1;

//Command Protocol:
//Operations and parameters are sent in 4 byte packets over DPTI.

//The operation is sent first:
//The opcode is sent in the lower 2 bytes
//The ZmodDAC channel is sent in the upper 2 bytes (bits 17&16)

//The parameter is sent second, if applicable

#ifndef bool
typedef enum { false, true } bool;
#endif

//Function flags
bool fWriteWave=false;
bool fStart=false;
bool fStop=false;
bool fSetFreq=false;
bool fSetGain=false;

//DPTI Initialized Flag
bool fDptiInit=false;

//Parameters
uint16_t divider;
uint8_t gain;
char filename[256];

//DPTI Device Variables
HIF hif;
DPRP pDevProperties;
int portNum=1; //Synchronous DPTI Port


//Forward Declarations
void closeDPTI();
int parseArgs(char* input);
void printUsage();
int getNumLines(FILE* csv);
int getWaveData(FILE* csv, BYTE* buf);
int initDPTI();

int main(int argc, char* argv[]){
	int arg;
	int nLines;
	BYTE* pBuf;
	unsigned int nVals;
	char input[256];
	BYTE opCodeToSend[4];
	BYTE paramToSend[4];
	int status;

	printUsage();
	atexit(closeDPTI);

	while(1){
		if(fDptiInit==false){
			if(status = initDPTI()!=0){
			printf("Error %d opening DPTI port\n", status);
			return status;
			}else{
				fDptiInit=true;
			}
		}
		printf("Channel %d:", channel);
		fgets(input, sizeof(input), stdin);
		if(parseArgs(input)!=0){
			continue;
		}
		

	//Write wave operation
		if (fWriteWave){
			fWriteWave=false;
			FILE* waveform = fopen(filename, "r");
			if (waveform==NULL){
				printf("Error: Failed to open %s\n", filename);
				continue;
			}
			//Get number of lines in csv file. This will be roughly how many samples are in the waveform.
			nLines = getNumLines(waveform);
			//Each sample will be 2 bytes (14 bits)
			pBuf = (BYTE *)malloc(sizeof(uint16_t)*nLines);
			if(pBuf ==NULL){
				printf("ERROR: Failed to allocate memory for sample buffer\n");
				return -1;
			}
			nVals = getWaveData(waveform, pBuf);
			fclose(waveform);

			if(nVals == 0){
				printf("ERROR: Failed to get waveform data from file\n");
				continue;
			}
			*(uint32_t*)opCodeToSend = (WRITEWAVE_OP | (channel<<16));
			*(uint32_t*)paramToSend = nVals*2;
			// Opcode is sent first
			if(!DptiIO(hif, opCodeToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending opcode.\n");
				free(pBuf);
				closeDPTI();
				continue;
			}
			//Then the length is sent
			if(!DptiIO(hif, paramToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending filesize.\n");
				free(pBuf);
				closeDPTI();
				continue;
			}
			//Then Send the buffer
			if(!DptiIO(hif, pBuf, nVals*2, NULL, 0, fFalse))
			{
				printf("Error sending waveform.\n");
				free(pBuf);
				closeDPTI();
				continue;
			}
			free(pBuf);
		}
	//Start operation	
		if(fStart){
			fStart=false;
			*(uint32_t*)opCodeToSend = START_OP | (channel<<16);
			// Only Opcode is sent
			if(!DptiIO(hif, opCodeToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending opcode.\n");
				closeDPTI();
				continue;
			}
		}
	//Stop Operation
		if(fStop){
			fStop=false;
			*(uint32_t*)opCodeToSend = STOP_OP | (channel<<16);
			// Only Opcode is sent
			if(!DptiIO(hif, opCodeToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending opcode.\n");
				closeDPTI();
				continue;
			}
		}
	//Set Output Frequency Divider
		if(fSetFreq){
			fSetFreq=false;
			*(uint32_t*)opCodeToSend = SET_FREQ_DIV_OP | (channel<<16);
			*(uint32_t*)paramToSend = divider;
			// Opcode is sent first
			if(!DptiIO(hif, opCodeToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending opcode.\n");
				closeDPTI();
				continue;
			}
			// Divider parameter send second
			if(!DptiIO(hif, paramToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending param.\n");
				closeDPTI();
				continue;
			}
		}
		//Set Gain
		if(fSetGain){
			fSetGain=false;
			*(uint32_t*)opCodeToSend = SET_GAIN_OP | (channel<<16);
			*(uint32_t*)paramToSend = gain;
			// Opcode is sent first
			if(!DptiIO(hif, opCodeToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending opcode.\n");
				closeDPTI();
				continue;
			}
			// Gain parameter send second
			if(!DptiIO(hif, paramToSend, 4, NULL, 0, fFalse))
			{
				printf("Error sending param.\n");
				closeDPTI();
				continue;
			}
		}
		
	}

	return 0;
}
void closeDPTI(){
	DptiDisable(hif);
	DmgrClose(hif);
	fDptiInit=false;
}
int parseArgs(char* input){
	char* arg;
	arg = strtok(input, " \n");
	if(arg==NULL){
		printf("\nPlease enter a command:");
		return -1;
	}
	while(arg!=NULL){
		if(strcmp(strlwr(arg), "write")==0){
			arg = strtok(NULL," \n");
			if(arg!=NULL){
				strcpy(filename, arg);
				fWriteWave = true;
			}
			else{
				printf("Error: No filename specified\n");
				return -1;
			}
		}
		else if(strcmp(strlwr(arg), "start")==0){
			fStart = true;
		}
		else if(strcmp(strlwr(arg), "stop") == 0){
			fStop = true;
		}
		else if(strcmp(strlwr(arg), "div") == 0){
			arg = strtok(NULL," \n");
			if(arg==NULL){
				printf("Please enter a divider value, EX: div 42\n");
				return -1;
			}
			if(arg[0]<'0' && arg[0]>'9'){
				printf("Invalid divider value \"%s\". Please enter a divider value. EX: div 42\n", arg);
			}
			divider = atoi(arg);
			if(divider>0 && divider < 0x4000){
				fSetFreq = true;
			}
			else{
				printf("Divider value must be between 1 and 16384\n");
				return -1;
			}
		}
		else if(strcmp(strlwr(arg), "gain") == 0){
			arg = strtok(NULL," \n");
			if(arg==NULL){
				printf("Please enter a gain value, EX: gain high\n");
				return -1;
			}
			if(strcmp(strlwr(arg), "low")==0 || arg[0]=='0'){
				gain = 0;
				fSetGain=true;
			}
			else if(strcmp(strlwr(arg), "high")==0 || arg[0]=='1'){
				gain = 1;
				fSetGain=true;
			}
			
			else{
				printf("Invalid gain value \"%s\". Expected low, 0, high, or 1\n", arg);
				return -1;
			}
		}
		else if(strcmp(strlwr(arg), "ch1") == 0){
			channel = 1;
		}
		else if(strcmp(strlwr(arg), "ch2") == 0){
			channel = 2;
		}
		else if(strcmp(strlwr(arg), "help") == 0 || arg[0]=='?'){
			printUsage();
		}
		else {
			printf("Error: Invalid argument %s\n", arg);
			return -1;
		}
		arg = strtok(NULL, " \n");
	}

	return 0;
}
void printUsage(){
	printf("USB104A7 ZmodDAC demo\n------------------------------\n");
	printf("Commands\n");
	printf("write [path to waveform.csv]\t-\tWrite waveform csv to ZmodDAC\n");
	printf("start\t\t-\tStart the ZmodDAC\n");
	printf("stop\t\t-\tStop the ZmodDAC\n");
	printf("ch1/ch2\t\t-\tSelect DAC channel\n");
	printf("gain [low,high,0,1]\t-\tSet the gain low or high\n\n");
	

}
int getNumLines(FILE* csv){
	int ch;
	int nLines=0;
	fseek(csv, 0, SEEK_SET);
	do{
		ch = fgetc(csv);
		if (ch=='\n')nLines++;
	}while(ch!=EOF);
	return nLines;
}

int getWaveData(FILE* csv, BYTE* buf){
	char line[256];
	int offset=0;
	float sample;
	fseek(csv, 0, SEEK_SET);
	while(fgets(line, 256, csv)!=NULL){
		if((line[0]==0 || line[0]<'0' || line[0]>'9') && line[0]!='-'){//If it is not a number, skip
			continue;
		}
		sample = atof(line);
		if (sample>1)sample=1;
		else if (sample<-1)sample=-1;
		*(int16_t*)&buf[offset*2] = (int16_t)(0x1FFF * sample);//Convert from percentage to 14 bit signed int value
		//Using offset*2 here to pack the values as 16 bit ints.
		offset++;
	}
	return offset;
}

int initDPTI(){
	int status;
	int cprtPti;
	//Open device
	if((status = DmgrOpen(&hif, "Usb104A7_DPTI"))==false){
		status = DmgrGetLastError();
		printf("Error %d opening Usb104A7_DPTI\n", status);
		return status;	
	}
	//Get DPTI port count on device
	if((status = DptiGetPortCount(hif, &cprtPti))==false){
		status = DmgrGetLastError();
		printf("Error %d getting DPTI port count\n", status);
		DmgrClose(hif);
		return status;
	}
	if(cprtPti == 0){
		printf("No DPTI ports found\n");
		return status;
	}
	if((status = DptiGetPortProperties(hif, portNum, &pDevProperties))==false){
		status = DmgrGetLastError();
		printf("Error %d getting port properties\n", status);
		DptiDisable(hif);
		DmgrClose(hif);
		return status;
	}
	if(pDevProperties & dprpPtiSynchronous == 0){
		printf("Error device does not support Synchronous Parallel Transfers\n");
		return -1;
	}
	//Enable DPTI bus
	if((status = DptiEnableEx(hif, portNum))==false){
		status = DmgrGetLastError();
		printf("Error %d enabling DPTI bus\n", status);
		DptiDisable(hif);
		DmgrClose(hif);
		return status;
	}
	DmgrSetTransTimeout(hif, 1000);
	return 0;
}
// int parseArgs(int argc, char** argv){
// 	int arg;
// 	for (arg=1; arg<argc; arg++){
// 		if(strcmp(strlwr(argv[arg]), "write")==0){
// 			cmd = WRITEWAVE;
// 			continue;
// 		}
// 		if(strcmp(strlwr(argv[arg]), "start")==0){
// 			cmd = START;
// 			continue;
// 		}
// 		if(strcmp(strlwr(argv[arg]), "stop")==0){
// 			cmd = STOP;
// 			continue;
// 		}
// 		if (strcmp(strlwr(argv[arg]), "-f")==0){
// 			arg++;
// 			if(arg>argc || argv[arg]==NULL){
// 				printf("ERROR: No filename specified!\n");
// 				return -1;
// 			}
// 			fFilename=true;
// 			strcpy(filename, argv[arg]);
// 			continue;
// 		}
// 		if (strcmp(strlwr(argv[arg]), "-div")){
// 			arg++;
// 			if(arg>argc || argv[arg]==NULL){
// 				printf("ERROR: No frequency specified!\n");
// 				return -2;
// 			}
// 			fDivider=true;
// 			strcpy(divider, argv[arg]);
// 			continue;
// 		}
// 		else{
// 			printUsage();
// 			return 1;
// 		}
// 	}
// 	return 0;
// }

