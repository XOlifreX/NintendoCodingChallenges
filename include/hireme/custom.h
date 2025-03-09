#ifndef H_CUSTOM
#define H_CUSTOM

#include "helpers.h"
#include "algorithm.h"
#include "chrono"

u8* ReverseLevel1_OneSolution(u8 wanted[16], int cycles = 256);
u8* ReverseLevel1_ManySolutions(u8 wanted[16], char* path, int blocksize, int cycles = 256);

#endif // H_CUSTOM
