#pragma once

// Initializes the original input file and the new file to which vert data will be moved
void InitializeFiles();
// Writes the data into the new file (vert data only)
void WriteInputToFile();
// Closes all the opened files
void CloseFiles();
