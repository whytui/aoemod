
#pragma once

#include <random>
#include <string>

#undef max

class Randomizer
{
public:
	Randomizer() {
		static std::random_device rd;
		static std::default_random_engine generator(rd()); // construct at startup to get a random seed.
		this->generator = &generator;
	};

	// Returns a random value 0-100
	int GetRandomPercentageValue() {
		return this->distribution_0_100(*this->generator);
	}
	// Returns a random value 1-100
	int GetRandomNonZeroPercentageValue() {
		return this->distribution_1_100(*this->generator);
	}

	// Returns a random value within range (min and max included)
	int GetRandomValue(int min, int max) {
		int intervalSize = max - min + 1;
		int rawValue = this->distribution_large(*this->generator) - 1; // in [0, max_int-1]
		float fixedValue = ((float)rawValue) / ((float)std::numeric_limits<int>::max()); // in [0, 1[
		int v = (int)(fixedValue * intervalSize); // in [0, intervalSize[
		return v + min; // in [min, max]
	}

	// Returns a random value within range (min and max included), using Normal distribution (Gauss)
	// sigma=2 (moderate, for 0-10 interval, 5 has ~20%, 3 has ~16%, 1 has ~4% ; 90% of the results are in 2-8 interval, 10% are for 0,1,9,10)
	int GetRandomValue_normal_moderate(int min, int max) {
		float intervalBound = (float)(max - min); // if 0-centered, interval is [-intervalBound/2;+intervalBound/2]
		//float intervalSize = intervalBound + 1;
		int securityCounter = 0;
		float rawFloat = -1;
		int intValue = -1;
		while ((intValue < min) || (intValue > max)) {
			rawFloat = this->normalDistrib_sigma2(*this->generator); // most values are in [0,10], around 94%
			float fixedValue = rawFloat - 5;// 0 centered
			fixedValue = fixedValue / 10; // most values in [-0.5;0.5], repartition unchanged
			fixedValue = fixedValue * intervalBound; // most values in [-interval/2;+interval/2], repartition unchanged
			fixedValue += (intervalBound / 2); //
			fixedValue += min; // in [min, max]
			intValue = int(round(fixedValue));
			if (++securityCounter > 1000) {
				assert(false && "ERROR: infinite loop in randomizer. Aborted.");
				return ((max - min) / 2) + min;
			}
		}
		return intValue;
	}
	// Returns a random value within range (min and max included), using Normal distribution (Gauss)
	// sigma=4 (quite flat, each bound value has ~half chance compared to median)
	int GetRandomValue_normal_moreFlat(int min, int max) {
		float intervalBound = (float)(max - min); // if 0-centered, interval is [-intervalBound/2;+intervalBound/2]
		//float intervalSize = intervalBound + 1;
		int securityCounter = 0;
		float rawFloat = -1;
		int intValue = -1;
		while ((intValue < min) || (intValue > max)) {
			rawFloat = this->normalDistrib_sigma4(*this->generator); // most values are in [0,10], around 88%
			float fixedValue = rawFloat - 5;// 0 centered
			fixedValue = fixedValue / 10; // most values in [-0.5;0.5], repartition unchanged
			fixedValue = fixedValue * intervalBound; // most values in [-interval/2;+interval/2], repartition unchanged
			fixedValue += (intervalBound / 2); //
			fixedValue += min; // in [min, max]
			intValue = int(round(fixedValue));
			if (++securityCounter > 1000) {
				assert(false && "ERROR: infinite loop in randomizer. Aborted.");
				return ((max - min) / 2) + min;
			}
		}
		return intValue;
	}


	std::string testRepartition() {
		const int nrolls = 10000;  // number of experiments
		const int nstars = 100;    // maximum number of stars to distribute
#define _testsize 50
		std::default_random_engine generator;
		std::normal_distribution<double> distribution(_testsize/2, 4.0);
		/* With sigma=4.0: median has 10 stars. median+-5 has 4 stars
		Last visible star is on median+-8. ~88% of the distribution is in [median-5;median+5] (11 positions)
		If restricted to [median-5;median+5] (total 79 stars), median has 13%, median+5 has 6% (1 position)
		*/
		std::string s;
		int p[_testsize] = {};
		for (int i = 0; i<nrolls; ++i) {
			double number = round(distribution(generator));
			if ((number >= 0.0) && (number<_testsize)) ++p[int(number)];
		}
		s += "normal distribution:\r\n";
		for (int i = 0; i<_testsize; ++i) {
			s += std::to_string(i);
			s += ": ";
			s += std::string(p[i] * nstars / nrolls, '*') + "\r\n";
		}
		return s;
	}

private:
	std::default_random_engine *generator; // Needs to be constructed at execution (startup) to get random seed.
	std::uniform_int_distribution<int> distribution_0_100 = std::uniform_int_distribution<int>(0, 100);
	std::uniform_int_distribution<int> distribution_1_100 = std::uniform_int_distribution<int>(1, 100);
	std::uniform_int_distribution<int> distribution_large = std::uniform_int_distribution<int>(1, std::numeric_limits<int>::max());
	std::normal_distribution<float> normalDistrib_sigma2 = std::normal_distribution<float>(5, 2);
	std::normal_distribution<float> normalDistrib_sigma4 = std::normal_distribution<float>(5, 4);
};


static Randomizer randomizer;
