#pragma once


FILE* inputVertData;
FILE* compressedVertData;

// The number of bits used to represent each floating point vert value
int bitsPerValue;
// The total number of values that a number can take in it's compressed format
int numberOfBuckets;
// The total number of floating point numbers in the vert data
int numberOfElements = 0;

// The value which has to be written in compressed bit stream
unsigned short valueToWrite = 0x0000;
// The buffer which stores data elements and writes them to the binary file
unsigned short shortOfData = 0x0000;
// The bits in the buffer which still haven't been filled completely
int bitsLeftInShort = 16;


// The maximum value in the vert data input
double minimumFloatValue;
// The minimum value in the vert data input
double maximumFloatValue;
// The magnitude of each bucket container of values
double bucketSize;

// Assigns file pointers for the file containing vert data and the file containing  bit stream
void InitializeFiles();
// Finds the maximum and minimum values in the input vert data and stores them in maximumFloatValue and minimumFloatValue
void CalculateMaxAndMin();
// Calulate what will be the size of the bucket
void CalculateBucketSize();
// Write the bit stream associated meta data to the compressed file
void WriteMetaData();

void WriteCompressedValues();
// Writes a integer to the binary bit stream by adding it the next available position in buffer
void WriteToBinaryFile(int);
// If the buffer has any data left, write it to the compressed file
void WriteRemainingBinaryData();
// Close both the input file and the output file containing the compressed data
void CloseFiles();