#include "RainDropVoice.h"
#include <cmath>
#include <algorithm>

namespace ThunderStorm::Domain::Rain {

    RainDropVoice::RainDropVoice() 
        : generatorId(0)
        , type(0)
        , amp(0.0f)
        , decay(0.0f)
        , panL(0.0f)
        , panR(0.0f)
        , active(false)
        , localRand(0)
        , sinePhase(0.0f)
        , sinePhaseStep(0.0f)
        , sineDecay(0.0f)
        , sineAmp(0.0f)
        , filterG(0.0f)
        , filterK(0.0f) 
    {}
    
    void RainDropVoice::Trigger(int genId, int dropType, float fc, float Q, float durationMs, float sampleRate, float volume, float pan, unsigned int seed) {
        generatorId = genId;
        type = dropType;
        amp = volume;
        localRand = seed;
        
        float durationSamples = (durationMs / 1000.0f) * sampleRate;
        decay = std::exp(-6.9077f / durationSamples); // Decays to -60dB (0.001) over duration
        
        // Limit cutoff frequency to a stable audio range
        if (fc < 100.0f) fc = 100.0f;
        if (fc > 12000.0f) fc = 12000.0f;
        
        filterG = 3.1415926535f * fc / sampleRate;
        filterK = 1.0f / Q;
        
        filter.Reset();
        
        if (type == 1) {
            // Ground tap gets a soft low-frequency sine thump component
            sinePhase = 0.0f;
            float gndFreq = 120.0f + 100.0f * (static_cast<float>(seed % 100) / 100.0f); // 120Hz to 220Hz
            sinePhaseStep = (2.0f * 3.1415926535f * gndFreq) / sampleRate;
            sineDecay = std::exp(-6.9077f / ((12.0f / 1000.0f) * sampleRate)); // decays in 12ms
            sineAmp = 0.4f;
        } else {
            sineAmp = 0.0f;
        }
        
        panL = std::sqrt(1.0f - pan);
        panR = std::sqrt(pan);
        active = true;
    }
    
    void RainDropVoice::Process(float& outL, float& outR, float sampleRate) {
        if (!active) return;
        
        // LCG for local noise generation inside the voice
        localRand = localRand * 196314165 + 907633515;
        float white = static_cast<float>(static_cast<int>(localRand)) / 2147483648.0f; // -1.0 to 1.0
        
        float sample = 0.0f;
        if (type == 0 || type == 2) {
            // Type 0 & 2: Filtered white noise with very fast envelope decay
            float filteredNoise = filter.ProcessBP(white, filterG, filterK);
            sample = filteredNoise * amp;
        } else if (type == 1) {
            // Type 1: Ground/Wood tap
            float filteredNoise = filter.ProcessBP(white, filterG, filterK);
            float thump = std::sin(sinePhase) * sineAmp;
            
            sample = (filteredNoise + thump) * amp;
            
            sinePhase += sinePhaseStep;
            sineAmp *= sineDecay;
        }
        
        outL += sample * panL;
        outR += sample * panR;
        
        // Main envelope decay
        amp *= decay;
        if (amp < 0.0001f) {
            active = false;
        }
    }

}
