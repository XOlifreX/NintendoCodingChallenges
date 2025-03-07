#ifndef H_HELPERS
#define H_HELPERS

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string.h>
#include <map>
#include <chrono>

#include "common.h"
#include "algorithm.h"


struct PreviousIndexProgressInfo {
	int iteration; // the i at that point
	u8 prevConfussion[32];
	u8 prevIndexes[32];
	std::vector<std::pair<u8, u8>> progress;
};

extern std::map<int, std::vector<std::pair<int, int>>> letterToXORPairs;

extern std::vector<u8> possibleValues;
extern std::vector<u8> possibleValuesP2;
extern std::vector<u8> impossibleValues;
extern std::vector<u8> impossibleValuesP2;
extern std::vector<u8> repeatingValues;

extern std::map<u8, std::vector<u8>> repeatingValueIndexes;

extern std::vector<PreviousIndexProgressInfo> previousProgress;

// Setup methods
void calculateAllPairsForASCIIChars();
void setupConfussionHelperValues();

void setupStartReverseLevel1(u8 start[16], u8 input[32]);

// Confusion helpers
bool hasImpossibleConfusionValue(u8 calculated[32]);
bool isConfusionImpossible(u8 c, bool secondHalf);
bool hasRepeatingConfusionValue(u8 calculated[32]);
u8 getRepeatingConfusionValuesCount(u8 calculated[32]);
bool isConfussionRepeating(u8 c);
std::vector<u8> getIndexesOfConfusion(u8 c);

// Confusion Index helpers
u8 getIndexFromConfussionValue(u8 value, bool secondHalf);
bool setupIndexesListFromCheckingRepeatingList(std::vector<std::pair<u8, u8>>& repeating, u8 output[32], u8 currentIndex = 0);
bool hasNoMoreRepeatingIndexesToCheck(std::vector<std::pair<u8, u8>>& repeating, u8 prevInput[32], u8 currentIndex = 0);

#endif // H_HELPERS
