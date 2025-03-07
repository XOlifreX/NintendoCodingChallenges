#ifndef H_ALGORITHM
#define H_ALGORITHM

#include <iostream>
#include <iomanip>
#include "common.h"

extern u8 confusion[512];

extern u32 diffusion[32];

extern u8 diffution_calculated[32 * 32];

// ******************************

void Forward(u8 c[32], u8 d[32], u8 s[512], u32 p[32], int cycles = 256);
void Forward_clean(u8 c[32], u8 d[32], u8 s[512], u32 p[32]);

// *****

void ForwardWithOutput(u8 _input[32], u8 _output[32], u8 _confusion[512], u32 _diffusion[32], int cycles = 256);


#endif // !H_ALGORITHM
