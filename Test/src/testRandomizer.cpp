#include "../include/testRandomizer.h"

#undef max


void checkRandomResult(int min, int max, int total, int executionCount) {
	float meanValue = ((float)total) / ((float)executionCount);
	float delta = (float)(max - min);
	float expectedMeanValue = min + (delta / 2);
	float absDiff = abs(expectedMeanValue - meanValue);
	float allowedDelta = delta / 100;
	assert(absDiff < allowedDelta);//, "The mean of randomizer results should be no more than 1% distance of medium value"
}

inline void assertValueLessThanPercentage(int value, int totalIterations, int refPercentage) {
	int valuePercentage = (value * 100) / totalIterations;
	assertShowValues(valuePercentage < refPercentage, refPercentage, valuePercentage);
}
inline void assertValueMoreThanPercentage(int value, int totalIterations, int refPercentage) {
	int valuePercentage = (value * 100) / totalIterations;
	assertShowValues(valuePercentage > refPercentage, refPercentage, valuePercentage);
}


// Entry point to test customROR randomizer
void test_randomizer() {
#define arraySize 20
	int numberOf[arraySize];
	for (int i = 0; i < arraySize; i++) { numberOf[i] = 0; }
	const int iterations = 10000;
	int total = 0;
	int count = 0;
	int randomValue;
	for (int i = 0; i < iterations; i++) {
		randomValue = randomizer.GetRandomValue_normal_moderate(12, 17);
		numberOf[randomValue]++;
		count++;
		total += randomValue;
	}
	checkRandomResult(12, 17, total, count);
	for (int i = 0; i < 12; i++) { 
		assert(numberOf[i] == 0);
	}
	assertValueLessThanPercentage(numberOf[12], iterations, 5);
	assertValueLessThanPercentage(numberOf[17], iterations, 5);
	assertValueLessThanPercentage(numberOf[14], iterations, 50);
	assertValueLessThanPercentage(numberOf[15], iterations, 50);
	assertValueMoreThanPercentage(numberOf[14], iterations, 30);
	assertValueMoreThanPercentage(numberOf[15], iterations, 30);

	for (int i = 0; i < arraySize; i++) { numberOf[i] = 0; }
	total = 0;
	count = 0;
	for (int i = 0; i < iterations; i++) {
		randomValue = randomizer.GetRandomValue_normal_moreFlat(12, 17);
		numberOf[randomValue]++;
		count++;
		total += randomValue;
	}
	checkRandomResult(12, 17, total, count);
	assertValueLessThanPercentage(numberOf[12], iterations, 18);
	assertValueLessThanPercentage(numberOf[17], iterations, 18);
	assertValueMoreThanPercentage(numberOf[12], iterations, 8);
	assertValueMoreThanPercentage(numberOf[17], iterations, 8);
	assertValueLessThanPercentage(numberOf[14], iterations, 35);
	assertValueLessThanPercentage(numberOf[15], iterations, 35);
	assertValueMoreThanPercentage(numberOf[14], iterations, 15);
	assertValueMoreThanPercentage(numberOf[15], iterations, 15);

	total = 0;
	count = 0;
	for (int i = 0; i < iterations; i++) {
		randomValue = randomizer.GetRandomValue(12, 17);
		count++;
		total += randomValue;
	}
	checkRandomResult(12, 17, total, count);

	total = 0;
	count = 0;
	for (int i = 0; i < iterations; i++) {
		randomValue = randomizer.GetRandomValue(101, 203);
		count++;
		total += randomValue;
	}
	checkRandomResult(101, 203, total, count);
	for (int i = 12; i <= 17; i++) {
		// Total random, just check there is not extreme, but there is no real expected !
		assertValueLessThanPercentage(numberOf[i], iterations, 30);
		assertValueMoreThanPercentage(numberOf[i], iterations, 8);
	}

}

