// This program reads the sequence of vert data from the text file and compresses it
// The compressed data is stored as a bit stream in a binary file

#define _CRT_SECURE_NO_WARNINGS

#define MAGIC_COMPRESSION_VALUE 0.000000000000001

#include<iostream>
#include<stdio.h>
#include<math.h>

#include "main.h"

int main(int argc, char * argv[])
{
	bitsPerValue = 5;
	if (argc > 1)
	{
		sscanf(argv[1], "%d", &bitsPerValue);
	}
	InitializeFiles();
	CalculateMaxAndMin();
	CalculateBucketSize();
	WriteMetaData();
	WriteCompressedValues();
	WriteRemainingBinaryData();
	CloseFiles();
	return 0;
}

void InitializeFiles()
{
	char fileName[30];
	// Open the file from which vert data will be read
	inputVertData = fopen("vertData.txt", "r");
	if (inputVertData == NULL)
	{
		std::cout << "File with vert data not found by compressor program.\n";
	}
	// Open a new file to which compressed data will written as a bit stream
	sprintf(fileName, "compressedVertData%d.bin", bitsPerValue);
	compressedVertData = fopen(fileName, "wb");
}

void CalculateMaxAndMin()
{
	int successfulInputRead;
	double currentDouble;
	// Check if the file has appropriate data values stored in it
	successfulInputRead = fscanf(inputVertData, "%lf", &currentDouble);
	if (successfulInputRead != 1)
	{
		std::cout << "Input vert data to compressor has no floating point numbers.\n";
		exit(0);
	}
	// Initialize maximum and minimum values
	maximumFloatValue = currentDouble;
	minimumFloatValue = currentDouble;
	// Check for minimum and maximum values in the entire file
	do 
	{
		numberOfElements++;
		if (currentDouble > maximumFloatValue)
		{
			maximumFloatValue = currentDouble;
			//std::cout << "Max value changed.\n";
		}
		if (currentDouble < minimumFloatValue)
		{
			minimumFloatValue = currentDouble;
			//std::cout << "Min value changed.\n";
		}
		// Read next value and check if it is valid
		successfulInputRead = fscanf(inputVertData, "%lf", &currentDouble);
	} while (successfulInputRead == 1);
	// Reset the input file pointer
	rewind(inputVertData);
}

void CalculateBucketSize()
{
	numberOfBuckets = (int)pow(2, bitsPerValue);
	bucketSize = (maximumFloatValue - minimumFloatValue) / numberOfBuckets;
}

void WriteMetaData()
{
	// The file's meta data will contain 
	//			The bits used to represent each value (4 bytes)
	//			The total number of compressed values (4 bytes)
	//			The minimum floating point value in data set (8 bytes)
	//			The size of each bucket of storing values (8 bytes)
	fwrite(&bitsPerValue, sizeof(int), 1, compressedVertData);
	fwrite(&numberOfElements, sizeof(int), 1, compressedVertData);
	fwrite(&minimumFloatValue, sizeof(double), 1, compressedVertData);
	fwrite(&bucketSize, sizeof(double), 1, compressedVertData);
}

void WriteCompressedValues()
{
	int successfulInputRead;
	double currentDouble;
	successfulInputRead = fscanf(inputVertData, "%lf", &currentDouble);
	// Loop for storing vert data as integers
	do
	{
		// Conversion of a currently read double to its corresponding integer value
		successfulInputRead = (int)((currentDouble - minimumFloatValue + MAGIC_COMPRESSION_VALUE ) / bucketSize);
		if (successfulInputRead == numberOfBuckets)
		{
			successfulInputRead--;
		}
		// Write the integer value to the binary file in form of a bit stream
		WriteToBinaryFile(successfulInputRead);
		// Read and check next input value
		successfulInputRead = fscanf(inputVertData, "%lf", &currentDouble);
	} while (successfulInputRead == 1);
}

void WriteToBinaryFile(int value)
{
	unsigned short tempShort;

	valueToWrite = (unsigned short)value;
	tempShort = valueToWrite;
	// Case: When the entire value can fit in the current short
	if (bitsLeftInShort >= bitsPerValue)
	{
		// Map value to the left most unallocated bits of the buffer
		tempShort = tempShort << (bitsLeftInShort - bitsPerValue);
		shortOfData |= tempShort;
		// Decrement the number of bits left in the buffer
		bitsLeftInShort -= bitsPerValue;
		// In case the short gets filled by the current value, write it to the compressed file's data
		if (bitsLeftInShort == 0)
		{
			fwrite(&shortOfData, sizeof(short), 1, compressedVertData);
			// Reset the shortOfData
			bitsLeftInShort = 16;
			shortOfData = 0x0000;
		}
		return;
	}
	// Case: When the entire value cannot fit in the current short
	if (bitsLeftInShort < bitsPerValue)
	{
		// Allocate the upper bits of data value to the least significant bits of the buffer
		tempShort = tempShort >> (bitsPerValue - bitsLeftInShort);
		shortOfData |= tempShort;
		// Write the shortOfData to binary file containing the bit stream
		fwrite(&shortOfData, sizeof(short), 1, compressedVertData);
		// Account for the bits of the value which couldn't be accommodated in the previous buffer output
		tempShort = valueToWrite;
		tempShort = tempShort << (16 - bitsPerValue + bitsLeftInShort );
		shortOfData = 0x0000;
		// Update the buffer
		shortOfData |= tempShort;
		bitsLeftInShort = 16 - bitsPerValue + bitsLeftInShort;
		return;
	}
}

void WriteRemainingBinaryData()
{
	if (shortOfData == 0x00)	return;
	fwrite(&shortOfData, sizeof(short), 1, compressedVertData);
}

void CloseFiles()
{
	fclose(inputVertData);
	fclose(compressedVertData);
}