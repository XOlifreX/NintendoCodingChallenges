#include "custom.h"

// **********

std::pair<u8*, std::vector<u8*>> ReverseLevel1(u8 wanted[16], bool manySolutions, int cycles, u8* initial = nullptr) {
    std::vector<u8*> foundSolutions;
    u8* startingPoint = (u8*)malloc(sizeof(u8) * 32);

    u8 indexes[32];
    u8 prevCalculatedConfusions[32];
    for (u8 j = 0; j < 32; j++) {
        indexes[j] = 0;
        prevCalculatedConfusions[j] = 0;
    }

    if (initial != nullptr) {
        memcpy(indexes, initial, sizeof(u8) * 32);
        memcpy(startingPoint, initial, sizeof(u8) * 32);
    }
    
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
            if (currentCheckingIndexes.size() > 0 && !hasNoMoreRepeatingIndexesToCheck(currentCheckingIndexes, prevCalculatedConfusions)) {
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

                i--;
                continue;
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
            if (foundSolutions.size() > 0)
                return { startingPoint, foundSolutions };

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


        // ******************************************************************************************************************************************************************************
        // ***** Setup solution found ***************************************************************************************************************************************************
        // ******************************************************************************************************************************************************************************
        if (i == (cycles - 1)) {
            std::vector<std::pair<u8, u8>> finalConfusionRepeatingIndexes;
            for (u8 j = 0; j < 32; j++) {
                u8 currentValue = calculatedConfusions[j];

                // Current is not a repearing value in the confusion.
                if (!isConfussionRepeating(currentValue)) {
                    indexes[j] = getIndexFromConfussionValue(currentValue, false);

                    continue;
                }

                // Current is repeating confusion. So we set the first index it appears in the confusion to check.
                finalConfusionRepeatingIndexes.push_back({ j, 0 });

                std::vector<u8> temp = getIndexesOfConfusion(currentValue);
                indexes[j] = temp[0];
            }

            // A calculated confusion on the last step can still have multiple possible indexes. This covers all of those index
            //     combinations to find each possible solution from the calculated confusion
            bool firstOneChecked = false;
            if (finalConfusionRepeatingIndexes.size() > 0) {
                do {
                    if (firstOneChecked) {
                        setupIndexesListFromCheckingRepeatingList(finalConfusionRepeatingIndexes, calculatedConfusions);

                        u8 count = 0;
                        for (u8 j = 0; j < 32; j++) {
                            if (count >= finalConfusionRepeatingIndexes.size())
                                continue;
                            if (j != finalConfusionRepeatingIndexes[count].first)
                                continue;

                            std::vector<u8> temp = getIndexesOfConfusion(calculatedConfusions[j]);
                            indexes[j] = temp[finalConfusionRepeatingIndexes[count].second];
                            count++;
                        }
                    }
                    else {
                        firstOneChecked = true;
                    }

                    // Sanity check found potential solution
                    u8* tInput = (u8*)malloc(sizeof(u8) * 32);
                    memcpy(tInput, indexes, sizeof(u8) * 32);

                    u8 output[32];
                    Forward(tInput, output, confusion, diffusion);
                    free(tInput);

                    if (memcmp(output, wanted, 16) == 0) {
#ifdef PRINT_INFO
                        std::cout << "Solution found (" << std::dec << foundSolutions.size() + 1 << ")" << std::endl;
#endif
                        u8* temp = (u8*)malloc(sizeof(u8) + 32);
                        memcpy(temp, indexes, sizeof(u8) * 32);

                        foundSolutions.push_back(temp);

                        if (!manySolutions)
                            return { startingPoint, foundSolutions };
                    }
                } while (!hasNoMoreRepeatingIndexesToCheck(finalConfusionRepeatingIndexes, calculatedConfusions));
            }
            else {
                // Sanity check found potential solution
                u8 output[32];
                Forward(indexes, output, confusion, diffusion);

                if (memcmp(output, wanted, 16) == 0) {
#ifdef PRINT_INFO
                    std::cout << "Solution found (" << std::dec << foundSolutions.size() + 1 << ")" << std::endl;
#endif
                    u8* temp = (u8*)malloc(sizeof(u8) + 32);
                    memcpy(temp, indexes, sizeof(u8) * 32);

                    foundSolutions.push_back(temp);

                    if (!manySolutions)
                        return { startingPoint, foundSolutions };
                }
            }

            // If previous index(es) are available, try those.
            if (currentCheckingIndexes.size() > 0 && !hasNoMoreRepeatingIndexesToCheck(currentCheckingIndexes, prevCalculatedConfusions)) {
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

                i--;
                continue;
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
        }
        else {
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

    std::pair<u8*, std::vector<u8*>> result = ReverseLevel1(wanted, false, cycles);

    free(result.first);
    return result.second.at(0);
}

// *****

std::pair<u8*, std::vector<u8*>> ReverseLevel1_ManySolutions(u8 wanted[16], int cycles) {
    std::cout << "Settings things up for the Level 1 calculator..." << std::endl;
    ReverseLevel1_Initialize(wanted);
    std::cout << "Setup finished! Calculating solution started..." << std::endl;

    std::pair<u8*, std::vector<u8*>> result = ReverseLevel1(wanted, true, cycles);

    return  result;
}

// *****

void ReverseLevel1_ManySolutions_ManyInitials(u8 wanted[16], char* name, int solutionCount, int cycles) {
    std::cout << "Settings things up for the Level 1 calculator..." << std::endl;
    ReverseLevel1_Initialize(wanted);
    std::cout << "Setup finished! Calculating solutions started..." << std::endl;

    std::vector<std::pair<u8*, std::vector<u8*>>> solutions;
    std::chrono::time_point<std::chrono::system_clock> start, stop;

    std::ofstream resultFile(name);
    int counter = 0;

    do {
        std::pair<u8*, std::vector<u8*>> result;

        start = std::chrono::system_clock::now();
        do {
            // u8 initial[32] = { 0xe1, 0x88, 0xa7, 0x88, 0x57, 0x42, 0x0e, 0x3e, 0xe2, 0x34, 0xca, 0x1c, 0xdd, 0x18, 0x49, 0x7b, 0x8a, 0x82, 0x05, 0x51, 0x21, 0xa4, 0x56, 0x84, 0xe8, 0xe3, 0xbe, 0xf9, 0xdf, 0x00, 0xef, 0xd8 };
            u8* initial = (u8*)malloc(sizeof(u8) * 32);
            setupStartReverseLevel1(wanted, initial);

#ifdef PRINT_INFO
            std::cout << "Trying out: ";
            std::cout << "[";
            for (u8 j = 0; j < 32; j++) {
                std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)initial[j];
                if (j != 31)
                    std::cout << ", ";
            }
            std::cout << "]" << std::endl << std::endl;
#endif

            result = ReverseLevel1(wanted, true, cycles, initial);
        } while (result.second.size() == 0);
        stop = std::chrono::system_clock::now();

        std::chrono::duration<double> elapsed_seconds = stop - start;
        std::time_t end_time = std::chrono::system_clock::to_time_t(stop);

        std::cout << "Iteration " << std::dec << (counter + 1) << "/" << solutionCount << " - Found " << result.second.size() << " solutions in " << elapsed_seconds.count() << " seconds." << std::endl;

        resultFile << "Starting point: " << std::endl;
        resultFile << "[";
        for (u8 j = 0; j < 32; j++) {
            resultFile << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)result.first[j];
            if (j != 31)
                resultFile << ", ";
        }
        resultFile << "]" << std::endl;

        std::vector<u8*> solutions = result.second;
        for (int j = 0; j < solutions.size(); j++) {
            resultFile << "    [";
            for (u8 k = 0; k < 32; k++) {
                resultFile << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)solutions[j][k];
                if (k != 31)
                    resultFile << ", ";
            }
            resultFile << "] ";
            resultFile << "[";
            for (u8 k = 0; k < 32; k++) {
                resultFile << std::dec << (int)solutions[j][k];
                if (k != 31)
                    resultFile << ", ";
            }
            resultFile << "]" << std::endl;
        }

        for (int j = 0; j < result.second.size(); j++)
            free(result.second[j]);
        free(result.first);

        resultFile << "============================================" << std::endl;

        counter++;
    } while (counter < solutionCount);

    resultFile.close();

    std::cout << "============================================" << std::endl;
    std::cout << "Finished! View all results in " << name << std::endl;
}

// **********
