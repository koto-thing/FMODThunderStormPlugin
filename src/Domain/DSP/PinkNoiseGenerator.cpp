#include "PinkNoiseGenerator.h"

namespace ThunderStorm::Domain::DSP {

    PinkNoiseGenerator::PinkNoiseGenerator(unsigned int seed) 
        : randSeed(seed)
        , runningSum(0.0f)
        , index(0) 
    {
        for (int i = 0; i < 12; i++) {
            rows[i] = 0.0f;
        }
    }

    float PinkNoiseGenerator::NextWhite() {
        randSeed = randSeed * 196314165 + 907633515;
        return static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f;
    }

    void PinkNoiseGenerator::Reset() {
        runningSum = 0.0f;
        index = 0;
        for (int i = 0; i < 12; i++) {
            rows[i] = 0.0f;
        }
    }

    float PinkNoiseGenerator::NextSample() {
        int last_idx = index;
        index = (index + 1) & 4095;
        
        int diff = last_idx ^ index;
        int i = 0;
        while (diff > 1) {
            diff >>= 1;
            i++;
        }
        
        if (i < 12) {
            runningSum -= rows[i];
            float new_val = NextWhite() / 12.0f;
            runningSum += new_val;
            rows[i] = new_val;
        }
        
        float white = NextWhite() / 12.0f;
        return runningSum + white;
    }

}
