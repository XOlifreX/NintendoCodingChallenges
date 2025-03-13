#ifndef H_CUSTOM
#define H_CUSTOM

#include "helpers.h"
#include "algorithm.h"
#include "chrono"

u8* ReverseLevel1_OneSolution(u8 wanted[16], int cycles = 256);
std::pair<u8*, std::vector<u8*>> ReverseLevel1_ManySolutions(u8 wanted[16], int cycles = 256);
void ReverseLevel1_ManySolutions_ManyInitials(u8 wanted[16], char* name, int blocksize, int cycles = 256);

#endif // H_CUSTOM
