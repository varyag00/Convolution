/*
Name:		J. Daniel Gonzalez
UCID:		10058656
Class:		CPSC 501
Ass:		4

Notes:		Program implements convolution reverb to a dry sound with an inpulse response, and outputs the sound as if it were playing in a concert hall.

Sources:	
*/


#include<stdio.h>

#pragma region Macros

#define REQ_ARGS 4

#pragma endregion

#pragma region Attributes

char* usageStr = "Usage: convolve <input file> <IRfile> <output file> ";
char* inputFile;
char* IRFile;
char* outputFile;

#pragma endregion 


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

	//TODO: delete this after debugging
	printf("input file: %s, IR file: %s, output file: %s\n", inputFile, IRFile, outputFile);		

	//read .wav files

	

	//perform convolution
	

	getchar();		//keeps terminal open (visual studio) 
}

//reads .WAV file
float* readWAVFile(char* input){

	int size
	
}



