#ifndef FMODTHUNDERSTORMPLUGIN_THUNDERSTORMENGINE_H
#define FMODTHUNDERSTORMPLUGIN_THUNDERSTORMENGINE_H

#include "ThunderStormParameters.h"
#include "../Framework/FMODAllocator.h"
#include "../Domain/DSP/LowPassFilter.h"
#include "../Domain/DSP/HighPassFilter.h"
#include "../Domain/DSP/StateVariableFilter.h"
#include "../Domain/DSP/PinkNoiseGenerator.h"
#include "../Domain/DSP/DynamicsCompressor.h"
#include "../Domain/Rain/RainDropVoice.h"
#include "../Domain/Thunder/LightningStrike.h"
#include "../Domain/Thunder/Rumbler.h"
#include "../Domain/Thunder/Afterimage.h"
#include "../Domain/Thunder/Deepener.h"
#include <vector>
#include <cmath>

namespace ThunderStorm::UseCase {

    class ThunderStormEngine {
    private:
        float sampleRate;
        float thunderIntensity; // Default intensity/duration coefficient
        void* dspState;         // Opaque pointer to FMOD_DSP_STATE
        
        unsigned int randSeed;
        
        // High quality ambient rain noise
        Domain::DSP::PinkNoiseGenerator rainPinkL;
        Domain::DSP::PinkNoiseGenerator rainPinkR;
        
        // Dedicated filters for Rumble and Shower to isolate DSP states
        Domain::DSP::LowPassFilter rumbleLP_L, rumbleLP_R;
        Domain::DSP::HighPassFilter rumbleHP_L, rumbleHP_R;
        Domain::DSP::LowPassFilter showerLP_L, showerLP_R;
        Domain::DSP::HighPassFilter showerHP_L, showerHP_R;
        
        // Granular raindrops array
        Domain::Rain::RainDropVoice rainDrops[64];
        
        // Liquid 1 and Liquid 2 Output Filters
        Domain::DSP::LowPassFilter liquid1LP_L, liquid1LP_R;
        Domain::DSP::HighPassFilter liquid1HP_L, liquid1HP_R;
        Domain::DSP::LowPassFilter liquid2LP_L, liquid2LP_R;
        Domain::DSP::HighPassFilter liquid2HP_L, liquid2HP_R;

        // 1. Multi-Strike Lightning model
        Domain::Thunder::LightningStrike strikes[5];
        
        // 2. Rumbler model
        Domain::Thunder::Rumbler rumbler;
        
        // 3. Afterimage model
        Domain::Thunder::Afterimage afterimage;
        
        // 4. Deepener model
        Domain::Thunder::Deepener deepener;

        // Feedback Delay Lines for rolling echo physics
        static const int DELAY_BUF_SIZE = 16384;
        float delayBufferL[16384];
        float delayBufferR[16384];
        int writeIdx;
        float fbFilterL;
        float fbFilterR;
        
        // Shared pink noise generator for thunder
        Domain::DSP::PinkNoiseGenerator thunderPinkL;
        Domain::DSP::PinkNoiseGenerator thunderPinkR;
        
        // Global master thunder delay timer
        int thunderDelaySamples;
        bool thunderTriggeredPending;

        // Final output muffle filters for Indoor/Outdoor simulation
        Domain::DSP::LowPassFilter outputMuffleL;
        Domain::DSP::LowPassFilter outputMuffleR;
        
        Domain::DSP::DynamicsCompressor compressor;
        
        // Atmospheric absorption filters
        Domain::DSP::StateVariableFilter airAbsorptionL;
        Domain::DSP::StateVariableFilter airAbsorptionR;
        
        // White noise for thunder
        float NextNoise() {
            randSeed = randSeed * 196314165 + 907633515;
            return static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f;
        }

    public:
        ThunderStormEngine(float sampleRate, float thunderIntensity, void* dspState)
            : sampleRate(sampleRate)
            , thunderIntensity(thunderIntensity)
            , dspState(dspState)
            , randSeed(12345)
            , rainPinkL(13579)
            , rainPinkR(24680)
            , thunderPinkL(54321)
            , thunderPinkR(65432)
            , thunderDelaySamples(0)
            , thunderTriggeredPending(false)
            , writeIdx(0)
            , fbFilterL(0.0f)
            , fbFilterR(0.0f)
        {
            for (int i = 0; i < 5; ++i) {
                strikes[i].active = false;
            }
            rumbler.active = false;
            afterimage.active = false;
            deepener.active = false;
            
            for (int i = 0; i < 16384; ++i) {
                delayBufferL[i] = 0.0f;
                delayBufferR[i] = 0.0f;
            }
        }
        
        ~ThunderStormEngine() = default;
        
        void Reset();
        
        void Process(float* outL, float* outR, unsigned int length, const ThunderStormParameters& params);
    };
}

#endif // FMODTHUNDERSTORMPLUGIN_THUNDERSTORMENGINE_H
