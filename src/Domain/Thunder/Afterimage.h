#ifndef THUNDERSTORM_DOMAIN_THUNDER_AFTERIMAGE_H
#define THUNDERSTORM_DOMAIN_THUNDER_AFTERIMAGE_H

#include "../DSP/LowPassFilter.h"
#include "../DSP/StateVariableFilter.h"

namespace ThunderStorm::Domain::Thunder {

    class Afterimage {
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
        Domain::DSP::LowPassFilter lp1;
        Domain::DSP::StateVariableFilter bpf; // 333Hz, Q=4 BPF

        Afterimage();
        
        void Reset();
        
        void Trigger(float thunderDistance, float sampleRate, unsigned int seed);
        
        void Process(float& dryThunderL, float& dryThunderR, float sampleRate, float thunderStrike);
    };

}

#endif // THUNDERSTORM_DOMAIN_THUNDER_AFTERIMAGE_H
