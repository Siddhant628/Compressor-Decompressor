#pragma once

// The binary input file to the decompressor which contains the bit stream
FILE* compressedVertDataBin;
// The output text file with vert list formed through decompression
FILE* recreatedVertDataText;
// The original text file containing the vert list
FILE* uncompressedVertDataText;

// The minimum value of a vertex from the input file
double minimumFloatValue;
// The magnitude of each bucket storage of data
double bucketSize;
// The mean of the squared errors in decompressed values
double meanSquaredError = 0;

// The total number of vert values in the data set
int numberOfElements;
// The number of bits used to represent each vert in the bit stream
int bitsPerValue;
// The number of bits which are included in the data set and are present in the last short
int bitsInLastShort = 0;
// A boolean to check if an additional short is present at the end of the data set
int additionalShortPresent = 0;
// The total number of shorts in the compressed bit stream which need to be read and processed
int numberOfShortsToRead;
// The number of bits in the buffer that still haven't been converted to their corresponding float
int bitsLeftToBeProcessed = 16;	
// A boolean to check if an additional short was read by the decompressor
int additionalShortRead = 0;

// Opens the output and input files required for the program to execute
void InitializeFiles();
// Process the data before bit stream which is required for decompression
void ProcessMetaData();
// Calculate the number of shorts present in the bit stream
void CalculateTheNumberOfDataShorts();
// Calculate the number of bits in the last short of the bit stream which are actual data
void CalculateBitsOfFinalShort();
// Decompresses the data read from the compressed input file
void RecreateData();
// Decompresses and integer to its corresponding double value
double DecompressToDouble(int);
// Writes the decompressed value to the list of vert in the output file
void WriteDecompressedValueToFile(double);
// Calculates the sum of the squared errors
void CalculateNetSquaredError(double);
// Calculates the mean of squared errors, or the RMS
double CalculateMeanSquaredError();
// Closes all the files
void CloseFiles();