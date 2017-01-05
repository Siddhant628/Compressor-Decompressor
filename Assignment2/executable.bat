@echo off
Debug\InputGenerator.exe
for /l %%a IN (5,1,16) DO (
Debug\Program1.exe %%a
Debug\Program2.exe %%a
)