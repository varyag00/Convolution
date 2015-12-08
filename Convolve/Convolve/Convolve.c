/*
Name:		J. Daniel Gonzalez
UCID:		10058656
Class:		CPSC 501
Ass:		4

Notes:		Program implements convolution reverb to a dry sound with an inpulse response, and outputs the sound as if it were playing in a concert hall.

Sources:	Main WAV file: http://soundbible.com/989-10-Second-Applause.html
			Impulse Response: http://www.echochamber.ch/index.php/tipps-freebies/impulseresponses.html
			Reading .WAV file (formatting): https://stackoverflow.com/questions/16075233/reading-and-processing-wav-file-data-in-c-c
											https://stackoverflow.com/questions/22059189/read-a-file-as-byte-array
*/

#pragma region Macros

#define REQ_ARGS 4
#define _CRT_SECURE_NO_DEPRECATE
#define RIFFHEADER 0x46464952
#define WAVRIFF 0x54651475
#define FORMAT 0x020746d66
#define LABELEDTEXT 0x478747C6
#define INSTRUMENTATION 0x478747C6
#define SAMPLE 0x6C706D73
#define FACT 0x47361666
#define DATA 0x61746164
#define JUNK 0x4b4e554a

#define PI 3.14159265358979
#define TWO_PI (2.0*PI)

#pragma endregion

#pragma region Imports

#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>

#pragma endregion



#pragma region Attributes

// IO
char* usageStr = "Usage: convolve <input file> <IRfile> <output file> ";
char* inputFile;
char* IRFile;
char* outputFile;

//WAV file attributes


#pragma endregion 





//reads .WAV file input
float* readWAVFile(char* input){

	//open and read file
	FILE* fileptr;
	char* buffer;
	long len;

	fileptr = fopen(input, "rb");
	fseek(fileptr, 0, SEEK_END);
	//get length of input file in bytes
	len = ftell(fileptr);
	rewind(fileptr);

	//malloc enough for file + \0
	buffer = (char *)malloc((len + 1)*sizeof(char)); 
	//read entire file, byte-by-byte
	fread(buffer, len, 1, fileptr); 
	fclose(fileptr);

	//current location in buffer
	int current = 0;

	//get chunk ID
	char chunkID[4];
	
	
	
	
	
	
	
	float retVal[10];	//just for test
	return retVal;
}





//writes a .WAV file after processing
void writeWAVFile(char* output){


}


//performs time-based convolution on two float arrays (Smith p. 112 - 115)




int main(int argc, char *argv[]){
	
	//check arguments
	if (argc != REQ_ARGS){
		printf(usageStr);
		return 0;
	}

	//set args
	inputFile = argv[1];
	IRFile = argv[2];
	outputFile = argv[3];

	//TODO: delete
	printf("input file: %s, IR file: %s, output file: %s\n", inputFile, IRFile, outputFile);		


	//read .wav files
	float* inputWAV;
	inputWAV = readWAVFile(inputFile);
	

	//perform time-based convolution 
	

	
	//write data to output .WAV file

	getchar();		//keeps terminal open (visual studio) 
}


