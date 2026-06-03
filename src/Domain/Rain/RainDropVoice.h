#ifndef THUNDERSTORM_DOMAIN_RAIN_RAINDROPVOICE_H
#define THUNDERSTORM_DOMAIN_RAIN_RAINDROPVOICE_H

#include "../DSP/StateVariableFilter.h"

namespace ThunderStorm::Domain::Rain {

    // Granular synthesizer voice modeling organic raindrop impacts near the listener
    class RainDropVoice {
    public:
        int generatorId; // 0 = Liquid 1, 1 = Liquid 2
        int type; // 0 = sharp surface tap, 1 = soft ground tap, 2 = wet tap
        float amp;
        float decay;
        float panL;
        float panR;
        bool active;
        
        unsigned int localRand;
        float sinePhase;
        float sinePhaseStep;
        float sineDecay;
        float sineAmp;
        
        Domain::DSP::StateVariableFilter filter;
        float filterG;
        float filterK;

        RainDropVoice();
        
        void Trigger(int genId, int dropType, float fc, float Q, float durationMs, float sampleRate, float volume, float pan, unsigned int seed);
        void Process(float& outL, float& outR, float sampleRate);
    };

}

#endif // THUNDERSTORM_DOMAIN_RAIN_RAINDROPVOICE_H
