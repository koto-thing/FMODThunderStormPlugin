#ifndef THUNDERSTORM_DOMAIN_THUNDER_RUMBLER_H
#define THUNDERSTORM_DOMAIN_THUNDER_RUMBLER_H

#include "../DSP/LowPassFilter.h"
#include "../DSP/StateVariableFilter.h"

namespace ThunderStorm::Domain::Thunder {

    class Rumbler {
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
        Domain::DSP::LowPassFilter lp2;
        float phaserPhase;
        float shState;
        Domain::DSP::StateVariableFilter hpf; // For the 300Hz, Q=9 HPF

        Rumbler();
        
        void Reset();
        
        void Trigger(float thunderDistance, float sampleRate, unsigned int seed);
        
        void Process(float& dryThunderL, float& dryThunderR, float sampleRate, float thunderRumble);
    };

}

#endif // THUNDERSTORM_DOMAIN_THUNDER_RUMBLER_H
