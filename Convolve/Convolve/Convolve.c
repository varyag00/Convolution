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

			Code used from:	http://pages.cpsc.ucalgary.ca/~laringha/501/Wavefiles/readwrite/main.c
*/

#pragma region Macros

#define REQ_ARGS 4
#define IRSIZE 6
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
#define TWO_PI (PI * 2)

#pragma endregion




#include <stdio.h>
#include <stdlib.h>

char chunkID[5];
int chunkSize;
char format[5];

char subChunk1ID[5];
int subChunk1Size;
short audioFormat;
short numChannels;
int sampleRate;
int byteRate;
short blockAlign;
short bitsPerSample;

int channelSize;
char subChunk2ID[5];
int subChunk2Size;

short* data;

void print()
{
	chunkID[5] = '\0';
	format[5] = '\0';
	subChunk1ID[5] = '\0';
	subChunk2ID[5] = '\0';

	printf("\n============= HEADER INFO =============\n");
	printf(" chunkID:%s\n", chunkID);
	printf(" chunkSize:%d\n", chunkSize);
	printf(" format:%s\n", format);
	printf(" subChunk1ID:%s\n", subChunk1ID);
	printf(" subChunk1Size:%d\n", subChunk1Size);
	printf(" audioFormat:%d\n", audioFormat);
	printf(" numChannels:%d\n", numChannels);
	printf(" sampleRate:%d\n", sampleRate);
	printf(" byteRate:%d\n", byteRate);
	printf(" blockAlign:%d\n", blockAlign);
	printf(" bitsPerSample:%d\n", bitsPerSample);
	printf(" subChunk2ID:%s\n", subChunk2ID);
	printf(" subChunk2Size:%d\n", subChunk2Size);
}

int loadWave(char* filename)
{
	FILE* in = fopen(filename, "rb");

	if (in != NULL)
	{
		printf("Reading %s...\n", filename);

		fread(chunkID, 1, 4, in);
		fread(&chunkSize, 1, 4, in);
		fread(format, 1, 4, in);

		//sub chunk 1
		fread(subChunk1ID, 1, 4, in);
		fread(&subChunk1Size, 1, 4, in);
		fread(&audioFormat, 1, 2, in);
		fread(&numChannels, 1, 2, in);
		fread(&sampleRate, 1, 4, in);
		fread(&byteRate, 1, 4, in);
		fread(&blockAlign, 1, 2, in);
		fread(&bitsPerSample, 1, 2, in);

		//read extra bytes
		if (subChunk1Size == 18)
		{
			short empty;
			fread(&empty, 1, 2, in);
		}

		//sub chunk 2
		fread(subChunk2ID, 1, 4, in);
		fread(&subChunk2Size, 1, 4, in);

		//read data		
		int bytesPerSample = bitsPerSample / 8;
		int numSamples = subChunk2Size / bytesPerSample;
		data = (short*)malloc(sizeof(short) * numSamples);

		//fread(data, 1, bytesPerSample*numSamples, in);

		int i = 0;
		short sample = 0;
		while (fread(&sample, 1, bytesPerSample, in) == bytesPerSample)
		{
			data[i++] = sample;
			sample = 0;
		}

		fclose(in);
		printf("Closing %s...\n", filename);
	}
	else
	{
		printf("Can't open file: %s\n", filename);
		return 0;
	}
	return 1;
}

//TODO: complete or delete
int loadIR(char* filename){

	return 1;
}

int saveWave(char* filename)
{
	FILE* out = fopen(filename, "wb");

	if (out != NULL)
	{
		printf("\nWriting %s...\n", filename);

		fwrite(chunkID, 1, 4, out);
		fwrite(&chunkSize, 1, 4, out);
		fwrite(format, 1, 4, out);

		//sub chunk 1
		fwrite(subChunk1ID, 1, 4, out);
		fwrite(&subChunk1Size, 1, 4, out);
		fwrite(&audioFormat, 1, 2, out);
		fwrite(&numChannels, 1, 2, out);
		fwrite(&sampleRate, 1, 4, out);
		fwrite(&byteRate, 1, 4, out);
		fwrite(&blockAlign, 1, 2, out);
		fwrite(&bitsPerSample, 1, 2, out);

		//read extra bytes
		if (subChunk1Size == 18)
		{
			short empty = 0;
			fwrite(&empty, 1, 2, out);
		}

		//sub chunk 2
		fwrite(subChunk2ID, 1, 4, out);
		fwrite(&subChunk2Size, 1, 4, out);

		//read data		
		int bytesPerSample = bitsPerSample / 8;
		int sampleCount = subChunk2Size / bytesPerSample;

		//impulse response - echo
		float IR[IRSIZE];
		IR[0] = 1.0;
		IR[1] = 1.0;
		IR[2] = 1.0;
		IR[3] = 1.0;
		IR[4] = 1.0;
		IR[5] = 1.0;

		//write the data
		float* newData = (float*)malloc(sizeof(float) * (sampleCount + IRSIZE - 1));
		float maxSample = -1;
		float MAX_VAL = 32767.f;	//FIXME: find based on bits per sample

		for (int i = 0; i<sampleCount; ++i)
		{
			//convolve
			for (int j = 0; j<IRSIZE; ++j)
				newData[i + j] += ((float)data[i] / MAX_VAL) * IR[j];

			//Keep track of max value for scaling
			if (i == 0)
				maxSample = newData[0];
			else if (newData[i] > maxSample)
				maxSample = newData[i];
		}

		//scale and re write the data
		for (int i = 0; i < sampleCount + IRSIZE - 1; ++i)
		{
			newData[i] = (newData[i] / maxSample);
			short sample = (short)(newData[i] * MAX_VAL);
			fwrite(&sample, 1, bytesPerSample, out);
		}

		//clean up
		free(newData);
		fclose(out);
		printf("Closing %s...\n", filename);
	}
	else
	{
		printf("Can't open file: %s\n", filename);
		return 0;
	}
	return 1;
}

int size;

//performs time-based convolution as outlined in (Smith p. 112 - 115)
void timeBased(char* input, char* IR, char* output){

}



int main(int argc, char* argv[])
{
	char* filename;
	char* IRName;
	char* outFilename;

	if (argc == REQ_ARGS){
		filename = argv[1];
		IRName = argv[2];
		outFilename = argv[3];
	
		//TODO: delete this
		printf("input file: %s, IR file: %s, output file: %s\n", filename, IRName, outFilename);

	}
	else
	{
		printf("Invalid Usage: convolve <input.wav> <IR.wav> <output.wav>\n");
		getchar();		//keeps terminal open (visual studio) 
		return -1;
	}

	getchar();		//keeps terminal open (visual studio) 

	//perform time-based convolution

	

	//if (loadWave(filename))
	//{
	//	print();
	//	saveWave(outFilename);
	//	free(data);
	//}
	//else
	//{
	//	getchar();		//keeps terminal open (visual studio) 
	//	return -1;
	//}

	//getchar();		//keeps terminal open (visual studio) 
	//return 0;
}



/* OLD CODE:

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

//WAV file attributes (from "Audio File Formats" handout)
char chunkID[4];
int chunkSize;
char format[4];
char subchunk1ID[4];
int subchunk1Size;
short int audioFormat;
short int numChannels;
int sampleRate;
int byteRate;
short int blockAlign;
short int bitsPerSample;
char subchunk2ID[4];
int subchunk2Size;		//note: this is the size of data in BYTES
char* data;
short int size;			//this is the size of data in 2-BYTES

#pragma endregion





//reads .WAV file input
float* readWAVFile(char* input){

//open and read file

FILE* fileptr;
char* buffer;
long len;

fileptr = fopen(input, "rb");
fseek(fileptr, 0, SEEK_END);
//length of input file in bytes
len = ftell(fileptr);
rewind(fileptr);

//malloc enough for file + \0
buffer = (char *)malloc((len + 1)*sizeof(char));	//TODO: might not need the +1 since we're not actually dealing with a "string"
//read entire file, byte-by-byte
fread(buffer, len, 1, fileptr);
fclose(fileptr);

//current location in buffer
int current = 0;

//get chunk ID
memcpy(chunkID, buffer + current, 4);
current += 4;

//get chunk size
memcpy(chunkSize, buffer + current, 4);
current += 4;

//get format
memcpy(format, buffer + current, 4);
current += 4;

//get subChunk1ID
memcpy(subchunk1ID, buffer + current, 4);
current += 4;

//get subChunk1Size
memcpy(subchunk1Size, buffer + current, 4);
current += 4;

//get audioFormat
memcpy(audioFormat, buffer + current, 2);
current += 2;

//get numChannels
memcpy(numChannels, buffer + current, 2);
current += 2;

//get sampleRate
memcpy(sampleRate, buffer + current, 4);
current += 4;

//get byteRate
memcpy(byteRate, buffer + current, 4);
current += 4;

//get blockAlign
memcpy(blockAlign, buffer + current, 2);
current += 2;

//get bitsPerSample
memcpy(bitsPerSample, buffer + current, 2);
current += 2;

//if subchunk1Size is 18 rather than 16, we have 2 bytes of junk
if (subchunk1Size == 18)
{
current += 2;
}

//get subchunk2ID
memcpy(subchunk2ID, buffer + current, 4);
current += 4;

//get subchunk2Size
memcpy(subchunk2Size, buffer + current, 4);
current += 4;

//get data
memcpy(data, buffer + current, subchunk2Size);
current += subchunk2Size;

//read data to create a return data array
size = subchunk2Size / 2;
float* returnData;
short temp = 0;

for (int i = 0; i < size; i++){
temp = data[i];
returnData[i] = temp / (pow(2,15) - 1);
//equalize anything under -1.0
if (returnData[i] < -1.0)
returnData[i] = -1.0;
}

return returnData;
}





//writes a .WAV file after processing
void writeWAVFile(char* output){


}


//performs time-based convolution on two float arrays (Smith p. 112 - 115)
float* timeConvolution(float* input){

}



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

*/