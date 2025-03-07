#include "custom.h"


u8* ReverseLevel1(u8 wanted[16], int cycles) {
    std::cout << "Wanted:" << std::endl << "[";
    for (u8 j = 0; j < 16; j++) {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)wanted[j];
        if (j != 15)
            std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // *************************
    // ***** SETUP *************
    // *************************
    std::cout << "Settings things up for the Level 1 calculator..." << std::endl;

    setupConfussionHelperValues();
    calculateAllPairsForASCIIChars();

    u8 indexes[32];
    u8 prevCalculatedConfusions[32];
    for (u8 j = 0; j < 32; j++) {
        indexes[j] = 0;
        prevCalculatedConfusions[j] = 0;
    }

    std::cout << "Setup finished! Calculating solution started..." << std::endl;
    
    // Keeps track of the repeating indexes which ones need to be used.
    std::vector<std::pair<u8, u8>> currentCheckingIndexes;

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
        if (i == 0) // First cycle, setup initial indexes from wanted.
            setupStartReverseLevel1(wanted, indexes);

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

                for (u8 j = 0; j < 32; j++)
                    prevCalculatedConfusions[j] = aPreviousProgress.prevConfussion[j];

                for (u8 j = 0; j < 32; j++)
                    indexes[j] = aPreviousProgress.prevIndexes[j];

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
            i = -1;
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

            for (u8 j = 0; j < 32; j++)
                progress.prevConfussion[j] = prevCalculatedConfusions[j];

            for (u8 j = 0; j < 32; j++)
                progress.prevIndexes[j] = indexes[j];

            std::vector<std::pair<u8, u8>> temp(currentCheckingIndexes);
            progress.progress = temp;
            previousProgress.push_back(progress);
        }

        currentCheckingIndexes.clear();

        for (u8 j = 0; j < 32; j++)
            prevCalculatedConfusions[j] = calculatedConfusions[j];

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

    return indexes;
}
