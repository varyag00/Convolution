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
http://pages.cpsc.ucalgary.ca/~laringha/501/Wavefiles/main.c
*/

#pragma region Macros

#define REQ_ARGS 4
#define IRSIZE 6
#define BITS_PER_SAMPLE   16
#define BYTES_PER_SAMPLE  (BITS_PER_SAMPLE/8)
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


#pragma region Input WAV Loading 

char WAVchunkID[5];
int WAVchunkSize;
char WAVformat[5];

char WAVsubChunk1ID[5];
int WAVsubChunk1Size;
short WAVaudioFormat;
short WAVnumChannels;
int WAVsampleRate;
int WAVbyteRate;
short WAVblockAlign;
short WAVbitsPerSample;

int WAVchannelSize;
char WAVsubChunk2ID[5];
int WAVsubChunk2Size;

short* WAVdata;

//loads data for WAV file
int loadWave(char* filename)
{
	FILE* in = fopen(filename, "rb");

	if (in != NULL)
	{
		printf("Reading %s...\n", filename);

		fread(WAVchunkID, 1, 4, in);
		fread(&WAVchunkSize, 1, 4, in);
		fread(WAVformat, 1, 4, in);

		//sub chunk 1
		fread(WAVsubChunk1ID, 1, 4, in);
		fread(&WAVsubChunk1Size, 1, 4, in);
		fread(&WAVaudioFormat, 1, 2, in);
		fread(&WAVnumChannels, 1, 2, in);
		fread(&WAVsampleRate, 1, 4, in);
		fread(&WAVbyteRate, 1, 4, in);
		fread(&WAVblockAlign, 1, 2, in);
		fread(&WAVbitsPerSample, 1, 2, in);

		//read extra bytes 
		if (WAVsubChunk1Size == 18)
		{
			short empty;
			fread(&empty, 1, 2, in);
		}

		//sub chunk 2
		fread(WAVsubChunk2ID, 1, 4, in);
		fread(&WAVsubChunk2Size, 1, 4, in);

		//read data		
		int bytesPerSample = WAVbitsPerSample / 8;
		int numSamples = WAVsubChunk2Size / bytesPerSample;
		WAVdata = (short*)malloc(sizeof(short) * numSamples);

		//fread(data, 1, bytesPerSample*numSamples, in);

		int i = 0;
		short sample = 0;
		while (fread(&sample, 1, bytesPerSample, in) == bytesPerSample)
		{
			WAVdata[i++] = sample;
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

#pragma endregion 

#pragma region IR Loading 

char IRchunkID[5];
int IRchunkSize;
char IRformat[5];

char IRsubChunk1ID[5];
int IRsubChunk1Size;
short IRaudioFormat;
short IRnumChannels;
int IRsampleRate;
int IRbyteRate;
short IRblockAlign;
short IRbitsPerSample;

int IRchannelSize;
char IRsubChunk2ID[5];
int IRsubChunk2Size;

short* IRdata;

//loads data for IR file
int loadIR(char* filename){

	FILE* in = fopen(filename, "rb");

	if (in != NULL)
	{
		printf("Reading %s...\n", filename);

		fread(IRchunkID, 1, 4, in);
		fread(&IRchunkSize, 1, 4, in);
		fread(IRformat, 1, 4, in);

		//sub chunk 1
		fread(IRsubChunk1ID, 1, 4, in);
		fread(&IRsubChunk1Size, 1, 4, in);
		fread(&IRaudioFormat, 1, 2, in);
		fread(&IRnumChannels, 1, 2, in);
		fread(&IRsampleRate, 1, 4, in);
		fread(&IRbyteRate, 1, 4, in);
		fread(&IRblockAlign, 1, 2, in);
		fread(&IRbitsPerSample, 1, 2, in);

		//read extra bytes 
		if (IRsubChunk1Size == 18)
		{
			short empty;
			fread(&empty, 1, 2, in);
		}

		//sub chunk 2
		fread(IRsubChunk2ID, 1, 4, in);
		fread(&IRsubChunk2Size, 1, 4, in);

		//read data		
		int bytesPerSample = IRbitsPerSample / 8;
		int numSamples = IRsubChunk2Size / bytesPerSample;
		IRdata = (short*)malloc(sizeof(short) * numSamples);

		//fread(data, 1, bytesPerSample*numSamples, in);

		int i = 0;
		short sample = 0;
		while (fread(&sample, 1, bytesPerSample, in) == bytesPerSample)
		{
			IRdata[i++] = sample;
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

#pragma endregion 

#pragma region Write WAV File

short* OUTdata;
int OUTSize;

int saveWave(char* filename)
{
	FILE* out = fopen(filename, "wb");

	if (out != NULL)
	{
		printf("\nWriting %s...\n", filename);

		fwrite(WAVchunkID, 1, 4, out);
		fwrite(&WAVchunkSize, 1, 4, out);
		fwrite(WAVformat, 1, 4, out);

		//sub chunk 1
		fwrite(WAVsubChunk1ID, 1, 4, out);
		fwrite(&WAVsubChunk1Size, 1, 4, out);
		fwrite(&WAVaudioFormat, 1, 2, out);
		fwrite(&WAVnumChannels, 1, 2, out);
		fwrite(&WAVsampleRate, 1, 4, out);
		fwrite(&WAVbyteRate, 1, 4, out);
		fwrite(&WAVblockAlign, 1, 2, out);
		fwrite(&WAVbitsPerSample, 1, 2, out);

		//read extra bytes
		if (WAVsubChunk1Size == 18)
		{
			short empty = 0;
			fwrite(&empty, 1, 2, out);
		}

		//sub chunk 2
		fwrite(WAVsubChunk2ID, 1, 4, out);
		fwrite(&WAVsubChunk2Size, 1, 4, out);

		//read data		
		int bytesPerSample = WAVbitsPerSample / 8;
		int sampleCount = OUTSize / bytesPerSample;

		//write outDATA
		short val;
		for (int i = 0; i < sampleCount; i++)
		{
			val = (short)(OUTdata[i] * (pow(2, 15) - 1));
			fwrite((char*)&val, 1, 2, out);
		}

		//clean up
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

#pragma endregion

void print()
{
	WAVchunkID[5] = '\0';
	WAVformat[5] = '\0';
	WAVsubChunk1ID[5] = '\0';
	WAVsubChunk2ID[5] = '\0';

	printf("\n============= HEADER INFO =============\n");
	printf(" chunkID:%s\n", WAVchunkID);
	printf(" chunkSize:%d\n", WAVchunkSize);
	printf(" format:%s\n", WAVformat);
	printf(" subChunk1ID:%s\n", WAVsubChunk1ID);
	printf(" subChunk1Size:%d\n", WAVsubChunk1Size);
	printf(" audioFormat:%d\n", WAVaudioFormat);
	printf(" numChannels:%d\n", WAVnumChannels);
	printf(" sampleRate:%d\n", WAVsampleRate);
	printf(" byteRate:%d\n", WAVbyteRate);
	printf(" blockAlign:%d\n", WAVblockAlign);
	printf(" bitsPerSample:%d\n", WAVbitsPerSample);
	printf(" subChunk2ID:%s\n", WAVsubChunk2ID);
	printf(" subChunk2Size:%d\n", WAVsubChunk2Size);
}

//performs time-based convolution using the "Input Side Algorithm" from Smith p. 112-115
void convolve(short x[], int N, short h[], int M, short y[], int P){

	printf("\nPerforming time-based convolution\n");

	if (P != (N + M - 1)){
		printf("Output signal vector is of wrong size");
		printf("Aborting convolution!");
		return;
	}

	//set output array to all 0's
	for (int n = 0; n < P; n++){
		y[n] = 0;
	}

	//do the thing
	for (int n = 0; n < N; n++){
		for (int m = 0; m < M; m++){
			y[n + m] += x[n] * h[m];
		}
	}
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
		return -1;
	}

	//input file
	int WAVError = loadWave(filename);

	//IR
	int IRError = loadIR(IRName);

	//check for errors in processing
	if (!WAVError || !IRError)
	{
		printf("Error occurred while loading one of the .WAV files\n");
		getchar();		//keeps terminal open (visual studio) 
		return -1;
	}

	//creating output
	OUTSize = WAVsubChunk2Size + IRsubChunk2Size - 1;

	//convolve
	convolve(WAVdata, WAVsubChunk2Size, IRdata, IRsubChunk2Size, OUTdata, OUTSize);

	//write output to file
	saveWave(outFilename);

	getchar();		//keeps terminal open (visual studio) 

}
