I would recommend my lead engineer to use 12 bits. This is primarily because the RMS error doesn't diminish significantly by increasing the number of bits beyond 12 bits. However, the size of the data in the file keeps on increasing linearly with the number of bits.

executable.bat is the file which will generate all the cases on launching

Assignment2.xlsx has the graph plot of RMS values vs. File sizes

--------------------------------------------------------

The .bat needs only verts.txt. 

It will generate a vertData.txt file with vert data only. (Using InputGenerator.exe)

Program1.exe would use vertData.txt and values from 5-16 to generate compressedVertData%d.bin

Program2.exe would use compressedVertData%d.bin to decompress the data and compare the values against those in value vertData.txt

The RMS error values are printed in the the code itself and can be viewed in cmd while executing

--------------------------------------------------------