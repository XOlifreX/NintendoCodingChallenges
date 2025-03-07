#include "custom.h"

u8 input[32] = {
    //change only this :

    // Reverse me fast
    // 0x66,0xd5,0x4e,0x28,0x5f,0xff,0x6b,0x53,0xac,0x3b,0x34,0x14,0xb5,0x3c,0xb2,0xc6,
    // 0xa4,0x85,0x1e,0x0d,0x86,0xc7,0x4f,0xba,0x75,0x5e,0xcb,0xc3,0x6e,0x48,0x79,0x8f

    // Hire me!!!!!!!!
    0xd5, 0x77, 0xe8, 0x9f, 0x3d, 0x78, 0x4a, 0x78, 0xd2, 0xb4, 0x00, 0xf8, 0x2a, 0xc3, 0xc3, 0x82,
    0x54, 0x18, 0x91, 0xa7, 0x39, 0x82, 0xfd, 0x0d, 0x64, 0x63, 0x8b, 0x0c, 0xbd, 0x8d, 0xd0, 0x64
};


int main_test() {
    u8 target[] = "Hire me!!!!!!!!";
    u8 output[32];

    Forward_clean(input, output, confusion, diffusion);

    std::cout << target << std::endl;
    std::cout << output << std::endl;

    return memcmp(output, target, 16); // => contact apply(at)nerd.nintendo.com
}

int main(int argc, char* argv[]) {
    // return main_test();

    std::chrono::time_point<std::chrono::system_clock> start, stop;

    // u8 target[16] = "Reverse me fast";
    u8 target[16] = "Hire me!!!!!!!!";

    start = std::chrono::system_clock::now();
    u8* temp = ReverseLevel1(target);
    stop = std::chrono::system_clock::now();

    u8 output[32];

    u8 newInput[32];
    for (u8 i = 0; i < 32; i++)
        newInput[i] = temp[i];

    std::cout << "============================================" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Using this found solution:" << std::endl << "[";
    for (u8 j = 0; j < 32; j++) {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)newInput[j];
        if (j != 31)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "======================" << std::endl;
    
    std::chrono::duration<double> elapsed_seconds = stop - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(stop);

    std::cout << "Found solution in " << elapsed_seconds.count() << " seconds." << std::endl;
    std::cout << "============================================" << std::endl;

    Forward(newInput, output, confusion, diffusion);

    std::cout << "Target:       " << target << std::endl;
    std::cout << "Calculated:   " << output << std::endl;

    std::cout << "Output:" << std::endl << "[";
    for (u8 j = 0; j < 16; j++) {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)output[j];
        if (j != 15)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "============================================" << std::endl;

    return memcmp(output, target, 16); // => contact apply(at)nerd.nintendo.com
}
