#include "helpers.h"

// ******************************

std::map<int, std::vector<std::pair<int, int>>> letterToXORPairs;

void calculateAllPairsForASCIIChars() {
	// Only use possible values from the confussion.
	for (int i = 0; i < 255; i++) {
		int charracter = i;

		std::vector<std::pair<int, int>> tempVector;

		for (int j = 0; j < 255; j++) {
			if (isConfusionImpossible(j, false))
				continue;
			for (int k = 0; k < 255; k++) {
				if (isConfusionImpossible(k, false))
					continue;

				int temp = j ^ k;
				if (temp != charracter)
					continue;

				if (std::count(possibleValues.begin(), possibleValues.end(), j) == 0)
					continue;
				if (std::count(possibleValuesP2.begin(), possibleValuesP2.end(), k) == 0)
					continue;

				tempVector.push_back({ j, k });
			}
		}

		letterToXORPairs.emplace(charracter, tempVector);
	}

	int i = 0;
}

// *****

std::vector<u8> possibleValues;
std::vector<u8> possibleValuesP2;
std::vector<u8> impossibleValues;
std::vector<u8> impossibleValuesP2;
std::vector<u8> repeatingValues;

void setupConfussionHelperValues() {
	// All possible values.
	for (int i = 0; i < 512; i++) {
		u8 val = confusion[i];
	
		if (i < 256) {
			if (std::count(possibleValues.begin(), possibleValues.end(), val))
				continue;
			possibleValues.push_back(val);
		}
		else {
			if (std::count(possibleValuesP2.begin(), possibleValuesP2.end(), val))
				continue;
			possibleValuesP2.push_back(val);
		}
	}

	// All impossible values.
	for (int i = 0; i < 512; i++) {
		bool found = false;

		for (int j = 0; j < 256; j++) {
			if (i != confusion[j])
				continue;
			found = true;
		}

		if (found)
			continue;

		if (i > 255)
			impossibleValuesP2.push_back(i);
		else
			impossibleValues.push_back(i);
	}

	// All repeating values. <-- top 256
	for (int i = 0; i < 256; i++) {
		int count = 0;

		for (int j = 0; j < 256; j++) {
			if (i != confusion[j])
				continue;
			count++;
		}

		if (count <= 1)
			continue;

		repeatingValues.push_back(i);
	}
}

// **********

void setupStartReverseLevel1(u8 start[16], u8 input[32]) {
	for (int i = 0; i < 16; i++) {
		u8 currentChar = start[i];
		int index1 = (i * 2);
		int index2 = (i * 2) + 1;

		std::pair<u8, u8> pairToUse;

		auto vector = letterToXORPairs.at(currentChar);
		srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
		int randomVal = rand();
		u8 randomIndex = randomVal % (vector.size() - 1);
		pairToUse = vector.at(randomIndex);

		u8 val1 = getIndexFromConfussionValue(pairToUse.first, false);
		u8 val2 = getIndexFromConfussionValue(pairToUse.second, true);

		input[index1] = val1;
		input[index2] = val2;
	}
}

// **********

bool hasImpossibleConfusionValue(u8 calculated[32]) {
	for (int i = 0; i < 32; i++) {
		if (std::find(impossibleValues.begin(), impossibleValues.end(), calculated[i]) == impossibleValues.end())
			continue;

		return true;
	}

	return false;
}

// *****

bool isConfusionImpossible(u8 c, bool secondHalf) {
	if (secondHalf)
		return std::find(impossibleValuesP2.begin(), impossibleValuesP2.end(), c) != impossibleValuesP2.end();

	return std::find(impossibleValues.begin(), impossibleValues.end(), c) != impossibleValues.end();
}

// *****

bool hasRepeatingConfusionValue(u8 calculated[32]) {
	for (int i = 0; i < 32; i++) {
		if (std::find(repeatingValues.begin(), repeatingValues.end(), calculated[i]) == repeatingValues.end())
			continue;

		return true;
	}

	return false;
}

// *****

u8 getRepeatingConfusionValuesCount(u8 calculated[32]) {
	int count = 0;
	for (int i = 0; i < 32; i++) {
		if (std::find(repeatingValues.begin(), repeatingValues.end(), calculated[i]) == repeatingValues.end())
			continue;

		count++;
	}

	return count;
}

// *****

bool isConfussionRepeating(u8 c) {
	return std::find(repeatingValues.begin(), repeatingValues.end(), c) != repeatingValues.end();
}

// *****

std::vector<u8> getIndexesOfConfusion(u8 c) {
	std::vector<u8> indexes;
	for (int i = 0; i < 256; i++) {
		if (c != confusion[i])
			continue;

		indexes.push_back(i);
	}

	return indexes;
}

// **********

u8 getIndexFromConfussionValue(u8 value, bool secondHalf) {
	u8 temp = value;
	while (true) {
		int i = secondHalf ? 256 : 0;
		int threshold = secondHalf ? 512 : 256;

		for (; i < threshold; i++) {
			if (confusion[i] != temp)
				continue;
			return i;
		}

		temp++;
	}
	
	throw std::invalid_argument("Calculated confusion does not exist in the confusion table!");
};

// *****

bool setupIndexesListFromCheckingRepeatingList(std::vector<std::pair<u8, u8>>& repeating, u8 prevInput[32], u8 currentIndex) {
	std::pair<u8, u8> currentChecking = repeating[currentIndex];

	u8 prevRepeatingConfusionValue = prevInput[currentChecking.first];
	std::vector<u8> repeatingValueIndexes = getIndexesOfConfusion(prevRepeatingConfusionValue);


	if (repeating.size() > currentIndex + 1) {
		bool finished = setupIndexesListFromCheckingRepeatingList(repeating, prevInput, currentIndex + 1);

		if (!finished)
			return false;
	}

	// End of possible indexes reached for current indexes.
	if ((currentChecking.second + 1) == repeatingValueIndexes.size()) {
		currentChecking.second = 0;
		repeating[currentIndex] = currentChecking;

		return true;
	}

	currentChecking.second++;
	repeating[currentIndex] = currentChecking;

	return false;
}

// *****

bool hasNoMoreRepeatingIndexesToCheck(std::vector<std::pair<u8, u8>>& repeating, u8 prevInput[32], u8 currentIndex) {
	std::pair<u8, u8> currentChecking = repeating[currentIndex];

	u8 prevRepeatingConfusionValue = prevInput[currentChecking.first];
	std::vector<u8> repeatingValueIndexes = getIndexesOfConfusion(prevRepeatingConfusionValue);


	if (repeating.size() > currentIndex + 1) {
		bool finished = hasNoMoreRepeatingIndexesToCheck(repeating, prevInput, currentIndex + 1);

		if (!finished)
			return false;
	}

	// End of possible indexes reached for current indexes.
	return (currentChecking.second + 1) == repeatingValueIndexes.size();
}

// **********

void snapShotFromIndexes(u8 confusionIndexes[32]) {
	u8 _input[32];
	for (u8 j = 0; j < 32; j++)
		_input[j] = 0;

	for (u8 j = 0; j < 32; j++) {
		for (u8 k = 0; k < 32; k++) {
			if (diffution_calculated[(j * 32) + k] == 0)
				continue;

			_input[j] ^= confusion[confusionIndexes[k]]; // XOR
		}
	}

#ifdef PRINT_INFO
	std::cout << "After Forward:" << std::endl << "[";
	for (u8 i = 0; i < 32; i++) {
		std::cout << "0x" << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)_input[i];
		if (i != 31)
			std::cout << ", ";
	}
	std::cout << "]" << std::endl;
#endif
}

// **********
