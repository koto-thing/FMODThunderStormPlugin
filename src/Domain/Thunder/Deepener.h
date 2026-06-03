#ifndef THUNDERSTORM_DOMAIN_THUNDER_DEEPENER_H
#define THUNDERSTORM_DOMAIN_THUNDER_DEEPENER_H

#include "../DSP/LowPassFilter.h"
#include "../DSP/HighPassFilter.h"

namespace ThunderStorm::Domain::Thunder {

    class Deepener {
    private:
        unsigned int randSeed;
        
        float NextNoise() {
            randSeed = randSeed * 196314165 + 907633515;
            return static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f;
        }

    public:
        bool active;
        int delaySamples;
        int elapsedSamples;
        float panL, panR;
        Domain::DSP::LowPassFilter lp1;  // LPF 60Hz
        Domain::DSP::HighPassFilter hp1; // HPF 30Hz
        Domain::DSP::LowPassFilter lp2;  // LPF 80Hz

        Deepener();
        
        void Reset();
        
        void Trigger(float thunderDistance, float sampleRate, unsigned int seed);
        
        void Process(float& dryThunderL, float& dryThunderR, float sampleRate, float thunderGrowl, float thunderDistance);
    };

}

#endif // THUNDERSTORM_DOMAIN_THUNDER_DEEPENER_H
