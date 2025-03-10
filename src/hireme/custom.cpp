#include "custom.h"

// **********

std::pair<u8*, std::vector<u8*>> ReverseLevel1(u8 wanted[16], int cycles, u8* initial = nullptr) {
    std::vector<u8*> foundSolutions;
    u8 startingPoint[32];

    u8 indexes[32];
    u8 prevCalculatedConfusions[32];
    for (u8 j = 0; j < 32; j++) {
        indexes[j] = 0;
        prevCalculatedConfusions[j] = 0;
    }

    if (initial != nullptr)
        memcpy(indexes, initial, sizeof(u8) * 32);
    
    // Keeps track of the repeating indexes which ones need to be used.
    std::vector<std::pair<u8, u8>> currentCheckingIndexes;
    std::vector<PreviousIndexProgressInfo> previousProgress;

    // *************************
    // ***** MAIN **************
    // *************************
    u8 currentIteration = -1;
    for (int i = 0; i < cycles; i++) {
#ifdef PRINT_INFO
        if (currentIteration != i) {
            std::cout << std::endl << "Iteration " << std::dec << (i + 1) << "/" << cycles << std::endl;
            currentIteration = i;
        }
#endif

        // *************************
        // ***** Setup Initial *****
        // *************************
        if (i == 0 && initial == nullptr) { // First cycle, setup initial indexes from wanted.
            setupStartReverseLevel1(wanted, indexes);
            memcpy(startingPoint, indexes, sizeof(u8) * 32);
        }

        // *************************
        // ***** ALGORITHM *********
        // *************************
        u8 calculatedConfusions[32];
        for (u8 j = 0; j < 32; j++)
            calculatedConfusions[j] = 0;

        for (u8 j = 0; j < 32; j++) {
            for (u8 k = 0; k < 32; k++) {
                if (diffution_calculated[(j * 32) + k] == 0)
                    continue;

                calculatedConfusions[j] ^= indexes[k]; // XOR
            }
        }

        // *************************
        // ***** Check validity ****
        // *************************
        if (hasImpossibleConfusionValue(calculatedConfusions)) {
            // If previous index(es) are available, try those.
            if (currentCheckingIndexes.size() > 0) {
                bool fullyChecked = setupIndexesListFromCheckingRepeatingList(currentCheckingIndexes, prevCalculatedConfusions);

                if (!fullyChecked) {
                    u8 count = 0;
                    for (u8 j = 0; j < 32; j++) {
                        if (count >= currentCheckingIndexes.size())
                            continue;
                        if (j != currentCheckingIndexes[count].first)
                            continue;

                        std::vector<u8> temp = getIndexesOfConfusion(prevCalculatedConfusions[j]);
                        indexes[j] = temp[currentCheckingIndexes[count].second];
                        count++;
                    }

                    i--;
                    continue;
                }
            }

            // Restart from the previous in progress index checks.
            if (previousProgress.size() > 0) {
                // *********************************************
                // Setup previous state
                PreviousIndexProgressInfo aPreviousProgress = previousProgress[previousProgress.size() - 1];

                i = aPreviousProgress.iteration;

                memcpy(prevCalculatedConfusions, aPreviousProgress.prevConfussion, sizeof(u8) * 32);
                memcpy(indexes, aPreviousProgress.prevIndexes, sizeof(u8) * 32);

                std::vector<std::pair<u8, u8>> temp(aPreviousProgress.progress);
                currentCheckingIndexes = temp;

                previousProgress.pop_back();

                // *********************************************
                // Setup next indexes to check in the next iteration.
                setupIndexesListFromCheckingRepeatingList(currentCheckingIndexes, prevCalculatedConfusions);

                u8 count = 0;
                for (u8 j = 0; j < 32; j++) {
                    if (count >= currentCheckingIndexes.size())
                        continue;
                    if (j != currentCheckingIndexes[count].first)
                        continue;

                    std::vector<u8> temp = getIndexesOfConfusion(prevCalculatedConfusions[j]);
                    indexes[j] = temp[currentCheckingIndexes[count].second];
                    count++;
                }

#ifdef PRINT_INFO
                std::cout << "Nothing more to do. Jumping back to iteration " << std::dec << i << std::endl;
#endif

                i--;
                continue;
                // *********************************************
            }

            // Try again
            if (initial != nullptr)
                return { initial, foundSolutions };

            i = -1;
            foundSolutions.clear();
            currentCheckingIndexes.clear();
            continue;
        }


#ifdef PRINT_INFO
        // *************************
        // ***** Print results *****
        // *************************
        std::cout << std::endl << "VALID VALUE FOUND" << std::endl;

        // Print Initial indexes
        std::cout << "Indexes:" << std::endl << "[";
        for (u8 j = 0; j < 32; j++) {
            std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)indexes[j];
            if (j != 31)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl;

        // Print calculated confusion
        std::cout << "Confusion:" << std::endl << "[";
        for (u8 j = 0; j < 32; j++) {
            std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)calculatedConfusions[j];
            if (j != 31)
                std::cout << ", ";
        }
        std::cout << "]" << std::endl;
#endif


        // *************************
        // ***** Setup next round **
        // *************************

        // Setup repeating indexes.
        if (currentCheckingIndexes.size() > 0 && !hasNoMoreRepeatingIndexesToCheck(currentCheckingIndexes, prevCalculatedConfusions)) {
            PreviousIndexProgressInfo progress;
            progress.iteration = i;

            memcpy(progress.prevConfussion, prevCalculatedConfusions, sizeof(u8) * 32);
            memcpy(progress.prevIndexes, indexes, sizeof(u8) * 32);

            std::vector<std::pair<u8, u8>> temp(currentCheckingIndexes);
            progress.progress = temp;
            previousProgress.push_back(progress);
        }

        currentCheckingIndexes.clear();

        memcpy(prevCalculatedConfusions, calculatedConfusions, sizeof(u8) * 32);

        for (u8 j = 0; j < 32; j++) {
            u8 currentValue = calculatedConfusions[j];

            // Current is not a repearing value in the confusion.
            if (!isConfussionRepeating(currentValue)) {
                indexes[j] = getIndexFromConfussionValue(currentValue, false);

                continue;
            }   

            // Current is repeating confusion. So we set the first index it appears in the confusion to check.
            currentCheckingIndexes.push_back({ j, 0 });

            std::vector<u8> temp = getIndexesOfConfusion(currentValue);
            indexes[j] = temp[0];
        }

#ifdef PRINT_INFO
        if (currentCheckingIndexes.size() > 0)
            std::cout << "CURRENT HAS REPEATING VALUES" << std::endl;
#endif

        if (i == (cycles - 1)) {
            u8* temp = (u8*)malloc(sizeof(u8) + 32);
            memcpy(temp, indexes, sizeof(u8) * 32);

            foundSolutions.push_back(temp);
        }
    }

    return { startingPoint, foundSolutions };
}

// **********

void ReverseLevel1_Initialize(u8 wanted[16]) {
    std::cout << "Wanted:" << std::endl << "[";
    for (u8 j = 0; j < 16; j++) {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)wanted[j];
        if (j != 15)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    setupConfussionHelperValues();
    calculateAllPairsForASCIIChars();
}

// **********

u8* ReverseLevel1_OneSolution(u8 wanted[16], int cycles) {
    std::cout << "Settings things up for the Level 1 calculator..." << std::endl;
    ReverseLevel1_Initialize(wanted);
    std::cout << "Setup finished! Calculating solution started..." << std::endl;

    std::pair<u8*, std::vector<u8*>> result = ReverseLevel1(wanted, cycles);

    return result.second.at(0);
}

// *****

u8* ReverseLevel1_ManySolutions(u8 wanted[16], char* path, int solutionCount, int cycles) {
    std::cout << "Settings things up for the Level 1 calculator..." << std::endl;
    ReverseLevel1_Initialize(wanted);
    std::cout << "Setup finished! Calculating solution started..." << std::endl;

    std::vector<u8*> solutions;
    std::vector<PreviousIndexProgressInfo> previousProgress;

    do {
        
    } while (solutions.size() < 50);

    return nullptr;
}

// **********
