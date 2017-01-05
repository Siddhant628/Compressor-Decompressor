// This program processes the data provided to the compressor and extracts the vert data from the text file
// The vert data is copied to another text file which will be used by the compressor

#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<stdio.h>

#include "main.h"

FILE* originalDataFile;
FILE* inputDataFile;

int main()
{
	InitializeFiles();
	WriteInputToFile();
	CloseFiles();
	return 0;
}

void InitializeFiles()
{
	// Open a new file to which vertex data will be written
	inputDataFile = fopen("vertData.txt", "w");
	// Open the original input data file
	originalDataFile = fopen("verts.txt", "r");
	// Check if the original input data file is present
	if (originalDataFile == NULL)
	{
		std::cout << "Original input data file not found.\n";
		exit(0);
	}
}

void WriteInputToFile()
{
	//Declare variables which will be read from original input file
	double x, y, z, u, v, r, g, b;
	int id, successfulRead = 10;
	char textureName[33];
	// Read the first set of data
	successfulRead = fscanf(originalDataFile, "%d: %lf %lf %lf %lf %lf %s %lf %lf %lf", &id, &x, &y, &z, &u, &v, textureName, &r, &g, &b);
	// Check if the set of data has appropriate format
	if (successfulRead != 10)
	{
		std::cout << "Original input data file is not formatted appropriately.\n";
		exit(0);
	}
	// Store the data and repeat the process until data isn't in same format
	do
	{
		fprintf(inputDataFile, "%.15lf %.15lf %.15lf ", x, y, z);
		successfulRead = fscanf(originalDataFile, "%d: %lf %lf %lf %lf %lf %s %lf %lf %lf", &id, &x, &y, &z, &u, &v, textureName, &r, &g, &b);
	} while (successfulRead == 10);
}

void CloseFiles()
{
	fclose(originalDataFile);
	fclose(inputDataFile);
}