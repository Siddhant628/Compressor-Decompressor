#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<stdio.h>

#include"main.h"



int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		sscanf(argv[1], "%d", &bitsPerValue);
	}
	double rootMeanSquaredError;
	InitializeFiles();
	ProcessMetaData();
	CalculateTheNumberOfDataShorts();
	RecreateData();
	rootMeanSquaredError = CalculateMeanSquaredError();
	std::cout << "RMS Value for " << bitsPerValue<< " = " <<rootMeanSquaredError<<std::endl;
	CloseFiles();
	return 0;
}

void InitializeFiles()
{
	char fileName[30];
	sprintf(fileName, "compressedVertData%d.bin", bitsPerValue);
	// Open the file with compressed binary data in it
	compressedVertDataBin = fopen(fileName, "rb");
	if (compressedVertDataBin == NULL)
	{
		std::cout << "Compressed vertex data (binary) not found by decompressor.\n";
		exit(0);
	}
	// Open the file with uncompressed vert data
	uncompressedVertDataText = fopen("vertData.txt","r");
	if (uncompressedVertDataText == NULL)
	{
		std::cout << "Uncompressed vertex data not found by decompressor.\n";
		exit(0);
	}
	// Initialize a new file to which the recreated vert data will be copied
	recreatedVertDataText = fopen("recreatedVertDataText.txt","w");
}

void ProcessMetaData()
{
	// Read meta data from the compressed binary file
	// The file's meta data will contain 
	//			The bits used to represent each value (4 bytes)
	//			The total number of compressed values (4 bytes)
	//			The minimum floating point value in data set (8 bytes)
	//			The size of each bucket of storing values (8 bytes)
	fread(&bitsPerValue, sizeof(int), 1, compressedVertDataBin);
	fread(&numberOfElements, sizeof(int), 1, compressedVertDataBin);
	fread(&minimumFloatValue, sizeof(double), 1, compressedVertDataBin);
	fread(&bucketSize, sizeof(double), 1, compressedVertDataBin);
}

void CalculateTheNumberOfDataShorts()
{
	// Calculate the total number of bits
	numberOfShortsToRead = bitsPerValue * numberOfElements;
	// Calculate the number of shorts
	numberOfShortsToRead = (int) (numberOfShortsToRead / 16);
	// Check if there is an additional short at the end
	if ((bitsPerValue*numberOfElements) % 16 != 0)
	{
		numberOfShortsToRead++;
		additionalShortPresent = 1;
	}
}

void RecreateData()
{
	// Takes value 1 in case a short was read successfully from the bit stream
	int successfulRead;
	// Temporary variable to store a decompressed value
	double decompressedValue;
	// The last short which was read from the binary file
	unsigned short shortFromFile;
	unsigned short tempShort, tempShort2;
	// A short which contains 1s for the positions corresponding to the size of 
	unsigned short shortOfBitSizeLength = 0xffff;
	shortOfBitSizeLength = shortOfBitSizeLength >> (16 - bitsPerValue);

	CalculateBitsOfFinalShort();
	// Decompression Loop
	while (numberOfShortsToRead > 0 || additionalShortPresent == 1)
	{
		// In case this is the last short
		if (numberOfShortsToRead == 0)
		{
			// Case: The last short has multiple values in it
			if (bitsLeftToBeProcessed > bitsPerValue)
			{
				tempShort = shortFromFile;
				tempShort = tempShort >> (16 - bitsInLastShort + bitsLeftToBeProcessed - bitsPerValue);
				tempShort = tempShort & shortOfBitSizeLength;
				bitsLeftToBeProcessed -= bitsPerValue;
				decompressedValue = DecompressToDouble((int)tempShort);
				CalculateNetSquaredError(decompressedValue);
				continue;
			}
			// Case: The last short has only 1 new value in it
			else if (bitsLeftToBeProcessed == bitsPerValue)
			{
				tempShort = shortFromFile;
				tempShort = tempShort >> (16 - bitsInLastShort + bitsLeftToBeProcessed - bitsPerValue);
				tempShort = tempShort & shortOfBitSizeLength;
				bitsLeftToBeProcessed -= bitsPerValue;
				decompressedValue = DecompressToDouble((int)tempShort);
				CalculateNetSquaredError(decompressedValue);
				break;
			}
			break;
		}
		// Don't read another short if an additional short was read
		if (additionalShortRead)
		{

			additionalShortRead = 0;
		}
		// Read the next short in bit stream
		else
		{
			successfulRead = fread(&shortFromFile, sizeof(short), 1, compressedVertDataBin);
			if (successfulRead != 1)
			{
				std::cout << "Binary file read failed by decompressor.\n";
				exit(0);
			}
			numberOfShortsToRead--;
			// If the current short is the additional short, continue with loop to handle it separately
			if (numberOfShortsToRead == 0 && additionalShortPresent == 1) 
			{ 
				continue; 
			}
		}
		// Extract values from the short
		tempShort = shortFromFile;
		// While the short has complete values in it, perform the following loop
		while (bitsLeftToBeProcessed >= bitsPerValue)
		{
			tempShort = tempShort >> (bitsLeftToBeProcessed - bitsPerValue);
			tempShort = tempShort & shortOfBitSizeLength;
			decompressedValue = DecompressToDouble((int)tempShort);
			CalculateNetSquaredError(decompressedValue);
			// Brings the next set of bits to the left most end
			tempShort = shortFromFile;
			bitsLeftToBeProcessed -= bitsPerValue;
		}
		// Case: Entire short has been processed by the above
		if (bitsLeftToBeProcessed == 0)
		{
			bitsLeftToBeProcessed = 16;
			// In case the following short is the last one
			if (numberOfShortsToRead == 0)
			{
				bitsLeftToBeProcessed = bitsInLastShort;
			}
			continue;
		}

		// Case: Some bits are left in the short which still have to be processed
		successfulRead = fread(&shortFromFile, sizeof(short), 1, compressedVertDataBin);
		if (successfulRead != 1)
		{
			std::cout << "Binary file read failed by decompressor.\n";
			exit(0);
		}
		// Store the next short in a temp variable
		additionalShortRead = 1;
		numberOfShortsToRead--;
		tempShort2 = shortFromFile;
		// Operations to get the value distributed over the two shorts
		tempShort2 = tempShort2 >> bitsLeftToBeProcessed;
		tempShort = tempShort << (16 - bitsLeftToBeProcessed);
		tempShort2 |= tempShort;
		tempShort2 = tempShort2 >> (16 - bitsPerValue);
		decompressedValue = DecompressToDouble((int)tempShort2);
		CalculateNetSquaredError(decompressedValue);
		// Update the number of bits left to be processed
		if (numberOfShortsToRead == 0)
		{
			bitsLeftToBeProcessed = bitsInLastShort  - bitsPerValue + bitsLeftToBeProcessed;
			continue;
		}
		bitsLeftToBeProcessed = 16 - bitsPerValue + bitsLeftToBeProcessed;

	}
}

void CalculateBitsOfFinalShort()
{
	bitsInLastShort = bitsPerValue * numberOfElements;
	bitsInLastShort = (bitsInLastShort % 16);
}

double DecompressToDouble(int valueFromFile)
{
	double decompressedValue;
	decompressedValue = valueFromFile * bucketSize;
	decompressedValue += minimumFloatValue;
	decompressedValue += (bucketSize / 2);
	WriteDecompressedValueToFile(decompressedValue);
	return decompressedValue;
}

void WriteDecompressedValueToFile(double decompressedValue)
{
	fprintf(recreatedVertDataText, "%.15lf ", decompressedValue);
}

void CalculateNetSquaredError(double decompressedValue)
{
	double originalValue, error;
	fscanf(uncompressedVertDataText, "%lf", &originalValue);
	error = decompressedValue - originalValue;
	error *= error;
	// Add the error squared to the net error (mean is calculated later)
	meanSquaredError += error;
}

double CalculateMeanSquaredError()
{
	meanSquaredError /= numberOfElements;
	return sqrt(meanSquaredError);
}

void CloseFiles()
{
	fclose(compressedVertDataBin);
	fclose(recreatedVertDataText);
	fclose(uncompressedVertDataText);
}