#ifndef THUNDERSTORM_DOMAIN_THUNDER_LIGHTNINGSTRIKE_H
#define THUNDERSTORM_DOMAIN_THUNDER_LIGHTNINGSTRIKE_H

#include "../DSP/StateVariableFilter.h"

namespace ThunderStorm::Domain::Thunder {

    struct ClapEvent {
        int delaySamples;
        int durationSamples;
        int elapsedSamples;
        float maxEnv;
        float fcStart;
        Domain::DSP::StateVariableFilter filter0;
        Domain::DSP::StateVariableFilter filter1;
        bool active;
    };

    class LightningStrike {
    private:
        unsigned int randSeed;
        
        float NextNoise() {
            randSeed = randSeed * 196314165 + 907633515;
            return static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f;
        }

    public:
        bool active;
        int delaySamples; // Global delay
        float panL, panR;
        bool isImpulse;
        ClapEvent claps[40];
        int numClaps;
        int impulseOffsets[20];
        int elapsedSamples;

        LightningStrike();
        
        void Reset();
        
        void Trigger(float thunderStrike, float thunderDistance, float sampleRate, bool impulse, unsigned int seed);
        
        void Process(float& dryThunderL, float& dryThunderR, float sampleRate);
    };

}

#endif // THUNDERSTORM_DOMAIN_THUNDER_LIGHTNINGSTRIKE_H
