#include "custom.h"

#include <fstream>


u8 input[32] = {
    //change only this :

    // Reverse me fast
    // 0x66,0xd5,0x4e,0x28,0x5f,0xff,0x6b,0x53,0xac,0x3b,0x34,0x14,0xb5,0x3c,0xb2,0xc6,
    // 0xa4,0x85,0x1e,0x0d,0x86,0xc7,0x4f,0xba,0x75,0x5e,0xcb,0xc3,0x6e,0x48,0x79,0x8f

    // Hire me!!!!!!!!
    0x25, 0x8e, 0xda, 0x1d, 0x43, 0x57, 0xbc, 0xdd, 0xa4, 0x0b, 0x09, 0xdf, 0x48, 0xc5, 0xf0, 0x88,
    0xeb, 0x6f, 0x47, 0xb7, 0x0e, 0x94, 0x69, 0x87, 0xbb, 0x47, 0x68, 0xe4, 0x48, 0x53, 0xaa, 0x43
};


int main_test() {
    u8 target[] = "Hire me!!!!!!!!";
    u8 output[32];

    Forward_clean(input, output, confusion, diffusion);

    std::cout << target << std::endl;
    std::cout << output << std::endl;

    return memcmp(output, target, 16); // => contact apply(at)nerd.nintendo.com
}

int oneSolutionLevel1() {
    std::chrono::time_point<std::chrono::system_clock> start, stop;

    // u8 target[16] = "Reverse me fast";
    u8 target[16] = "Hire me!!!!!!!!";

    start = std::chrono::system_clock::now();
    u8* temp = ReverseLevel1_OneSolution(target);
    stop = std::chrono::system_clock::now();

    u8 result[32];
    memcpy(result, temp, sizeof(u8) * 32);
    free(temp);

    std::cout << "============================================" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Using this found solution:" << std::endl << "[";
    for (u8 j = 0; j < 32; j++) {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)result[j];
        if (j != 31)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "======================" << std::endl;

    std::chrono::duration<double> elapsed_seconds = stop - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(stop);

    std::cout << "Found solution in " << elapsed_seconds.count() << " seconds." << std::endl;
    std::cout << "============================================" << std::endl;

    u8 output[32];
    Forward(result, output, confusion, diffusion);

    std::cout << "Target:       " << target << std::endl;
    std::cout << "Calculated:   " << output << std::endl;

    std::cout << "Output: [";
    for (u8 j = 0; j < 16; j++) {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)output[j];
        if (j != 15)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "============================================" << std::endl;

    return memcmp(output, target, 16); // => contact apply(at)nerd.nintendo.com
}

int manySolutionsLevel1() {
    std::chrono::time_point<std::chrono::system_clock> start, stop;

    // u8 target[16] = "Reverse me fast";
    u8 target[16] = "Hire me!!!!!!!!";

    std::cout << "Target: " << std::endl;
    for (u8 i = 0; i < 16; i++)
        std::cout << target[i];
    std::cout << std::endl << "============================================" << std::endl;

    start = std::chrono::system_clock::now();
    std::pair<u8*, std::vector<u8*>> result = ReverseLevel1_ManySolutions(target);
    stop = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = stop - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(stop);

    std::vector<u8*> solutions = result.second;

    std::cout << "============================================" << std::endl;
    std::cout << "Found solutions in " << elapsed_seconds.count() << " seconds." << std::endl;
    std::cout << std::dec << solutions.size() << " solutions found!!" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "============================================" << std::endl;

    for (int i = 0; i < solutions.size(); i++) {
        std::cout << "Solution " << std::dec << i + 1 << ":" << std::endl;
        u8 result[32];
        memcpy(result, solutions[i], sizeof(u8) * 32);
        free(solutions[i]);

        std::cout << "[";
        for (u8 j = 0; j < 32; j++) {
            std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)result[j];
            if (j != 31)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        std::cout << "======================" << std::endl;

        u8 output[32];
        Forward(result, output, confusion, diffusion);

        std::cout << "Target:       " << target << std::endl;
        std::cout << "Calculated:   " << output << std::endl;

        std::cout << "Output: [";
        for (u8 j = 0; j < 16; j++) {
            std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)output[j];
            if (j != 15)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        std::cout << "============================================" << std::endl;
    }

    free(result.first);

    return 0;
}

int manySolutionsCustomManyInitialsInitialLevel1() {
    std::chrono::time_point<std::chrono::system_clock> start, stop;

    // u8 target[16] = "Reverse me fast";
    u8 target[16] = "Hire me!!!!!!!!";

    std::cout << "Target: " << std::endl;
    for (u8 i = 0; i < 16; i++)
        std::cout << target[i];
    std::cout << std::endl << "============================================" << std::endl;

    char fileName[24] = "HireMe_10Solutions.txt";
    ReverseLevel1_ManySolutions_ManyInitials(target, fileName, 10);

    return 0;
}

int main(int argc, char* argv[]) {
    // return main_test();
    // return oneSolutionLevel1();
    // return manySolutionsLevel1();
    return manySolutionsCustomManyInitialsInitialLevel1();
}