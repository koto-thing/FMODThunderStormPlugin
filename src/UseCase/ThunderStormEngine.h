#ifndef FMODTHUNDERSTORMPLUGIN_THUNDERSTORMENGINE_H
#define FMODTHUNDERSTORMPLUGIN_THUNDERSTORMENGINE_H

#include "ThunderStormParameters.h"
#include "../Framework/FMODAllocator.h"
#include <vector>
#include <cmath>

namespace ThunderStorm::UseCase {

    // 1/f Pink Noise generator using the Voss-McCartney algorithm
    class PinkNoiseGenerator {
    private:
        unsigned int randSeed;
        float rows[12];
        float runningSum;
        int index;

        float NextWhite() {
            randSeed = randSeed * 196314165 + 907633515;
            return static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f;
        }

    public:
        PinkNoiseGenerator(unsigned int seed) : randSeed(seed), runningSum(0.0f), index(0) {
            for (int i = 0; i < 12; i++) {
                rows[i] = 0.0f;
            }
        }

        void Reset() {
            runningSum = 0.0f;
            index = 0;
            for (int i = 0; i < 12; i++) {
                rows[i] = 0.0f;
            }
        }

        float NextSample() {
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
    };

    // State Variable Filter (SVF) for shaping raindrop resonance and thunder components
    struct SVF {
        float ic1eq;
        float ic2eq;
        
        SVF() : ic1eq(0.0f), ic2eq(0.0f) {}
        void Reset() { ic1eq = 0.0f; ic2eq = 0.0f; }
        
        // Standard Trapezoidal SVF for stable bandpass output
        float ProcessBP(float input, float g, float k) {
            float v3 = input - ic2eq;
            float v1 = (g * v3 + ic1eq) / (1.0f + g * (g + k));
            float v2 = ic2eq + g * v1;
            ic1eq = 2.0f * v1 - ic1eq;
            ic2eq = 2.0f * v2 - ic2eq;
            return v1; 
        }

        // Returns Lowpass, Bandpass, Highpass outputs
        void ProcessMulti(float input, float g, float k, float& lp, float& bp, float& hp) {
            float v3 = input - ic2eq;
            float v1 = (g * v3 + ic1eq) / (1.0f + g * (g + k));
            float v2 = ic2eq + g * v1;
            ic1eq = 2.0f * v1 - ic1eq;
            ic2eq = 2.0f * v2 - ic2eq;
            
            bp = v1;
            lp = v2;
            hp = input - k * v1 - v2;
        }
    };

    // Granular synthesizer voice modeling organic raindrop impacts near the listener
    struct RainDropVoice {
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
        
        SVF filter;
        float filterG;
        float filterK;

        RainDropVoice() : generatorId(0), type(0), amp(0.0f), decay(0.0f), panL(0.0f), panR(0.0f), active(false), localRand(0), sinePhase(0.0f), sinePhaseStep(0.0f), sineDecay(0.0f), sineAmp(0.0f), filterG(0.0f), filterK(0.0f) {}
        
        void Trigger(int genId, int dropType, float fc, float Q, float durationMs, float sampleRate, float volume, float pan, unsigned int seed) {
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
        
        void Process(float& outL, float& outR, float sampleRate) {
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
                // Type 1: Ground/Wood tap (Soft thump + filtered noise)
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
    };

    // Simple 1st-order Low-pass Filter
    struct LowPassFilter {
        float state;
        LowPassFilter() : state(0.0f) {}
        void Reset() { state = 0.0f; }
        float Process(float input, float coef) {
            state += coef * (input - state);
            return state;
        }
    };

    // Simple 1st-order High-pass Filter
    struct HighPassFilter {
        float state;
        float prevInput;
        HighPassFilter() : state(0.0f), prevInput(0.0f) {}
        void Reset() { state = 0.0f; prevInput = 0.0f; }
        float Process(float input, float coef) {
            float output = input - prevInput + coef * state;
            prevInput = input;
            state = output;
            return output;
        }
    };
    
    // Dynamic Range Compressor (AES paper model)
    struct DynamicsCompressor {
        float thresholdDb;
        float kneeDb;
        float ratio;
        float attackSec;
        float releaseSec;
        float sampleRate;
        float currentGainDb;

        DynamicsCompressor() 
            : thresholdDb(-20.0f)
            , kneeDb(20.0f)
            , ratio(12.0f)
            , attackSec(0.0f)
            , releaseSec(0.5f)
            , sampleRate(48000.0f)
            , currentGainDb(0.0f)
        {}

        void Reset() {
            currentGainDb = 0.0f;
        }

        void Process(float& inL, float& inR) {
            float inVal = std::max(std::abs(inL), std::abs(inR));
            float inDb = 20.0f * std::log10(inVal + 1e-9f);

            float targetGainDb = 0.0f;
            float diff = inDb - thresholdDb;

            if (diff > kneeDb * 0.5f) {
                targetGainDb = diff * (1.0f / ratio - 1.0f);
            } else if (diff > -kneeDb * 0.5f) {
                float num = diff + kneeDb * 0.5f;
                targetGainDb = num * num * (1.0f / ratio - 1.0f) / (2.0f * kneeDb);
            }

            if (targetGainDb < currentGainDb) {
                currentGainDb = targetGainDb;
            } else {
                float releaseCoef = 1.0f - std::exp(-1.0f / (releaseSec * sampleRate));
                currentGainDb += releaseCoef * (targetGainDb - currentGainDb);
            }

            float gainLinear = std::pow(10.0f, currentGainDb / 20.0f);
            inL *= gainLinear;
            inR *= gainLinear;
        }
    };

    class ThunderStormEngine {
    private:
        float sampleRate;
        float thunderIntensity; // Default intensity/duration coefficient
        void* dspState;         // Opaque pointer to FMOD_DSP_STATE
        
        unsigned int randSeed;
        
        // High quality ambient rain noise
        PinkNoiseGenerator rainPinkL;
        PinkNoiseGenerator rainPinkR;
        
        // Dedicated filters for Rumble and Shower to isolate DSP states
        LowPassFilter rumbleLP_L, rumbleLP_R;
        HighPassFilter rumbleHP_L, rumbleHP_R;
        LowPassFilter showerLP_L, showerLP_R;
        HighPassFilter showerHP_L, showerHP_R;
        
        // Granular raindrops array (64 voices for concurrent generators)
        RainDropVoice rainDrops[64];
        
        // Liquid 1 and Liquid 2 Output Filters
        LowPassFilter liquid1LP_L, liquid1LP_R;
        HighPassFilter liquid1HP_L, liquid1HP_R;
        LowPassFilter liquid2LP_L, liquid2LP_R;
        HighPassFilter liquid2HP_L, liquid2HP_R;

        // 1. Multi-Strike Lightning model (AES Paper)
        struct ClapEvent {
            int delaySamples;
            int durationSamples;
            int elapsedSamples;
            float maxEnv;
            float fcStart;
            SVF filter0;
            SVF filter1;
            bool active;
        };

        struct LightningStrike {
            bool active;
            int delaySamples; // Global delay (distance delay + random offset)
            float panL, panR;
            bool isImpulse;
            ClapEvent claps[40];
            int numClaps;
            int impulseOffsets[20];
            int elapsedSamples;
        };
        LightningStrike strikes[5];
        
        // 2. Rumbler model
        struct RumblerState {
            bool active;
            int delaySamples;
            int elapsedSamples;
            float panL, panR;
            LowPassFilter lp1;
            LowPassFilter lp2;
            float phaserPhase;
            float shState;
            SVF hpf; // For the 300Hz, Q=9 HPF
        };
        RumblerState rumbler;
        
        // 3. Afterimage model
        struct AfterimageState {
            bool active;
            int delaySamples;
            int elapsedSamples;
            float panL, panR;
            LowPassFilter lp1;
            SVF bpf; // 333Hz, Q=4 BPF
        };
        AfterimageState afterimage;
        
        // 4. Deepener model (Growl boost)
        struct DeepenerState {
            bool active;
            int delaySamples;
            int elapsedSamples;
            float panL, panR;
            LowPassFilter lp1;  // LPF 60Hz
            HighPassFilter hp1; // HPF 30Hz
            LowPassFilter lp2;  // LPF 80Hz
        };
        DeepenerState deepener;

        // Feedback Delay Lines for rolling echo physics (Nyquist paper model)
        static const int DELAY_BUF_SIZE = 16384;
        float delayBufferL[16384];
        float delayBufferR[16384];
        int writeIdx;
        float fbFilterL;
        float fbFilterR;
        
        // Shared pink noise generator for thunder
        PinkNoiseGenerator thunderPinkL;
        PinkNoiseGenerator thunderPinkR;
        
        // Global master thunder delay timer (delay before the first/main strike hits)
        int thunderDelaySamples;
        bool thunderTriggeredPending;

        // Final output muffle filters for Indoor/Outdoor simulation
        LowPassFilter outputMuffleL;
        LowPassFilter outputMuffleR;
        
        DynamicsCompressor compressor;
        
        // Atmospheric absorption filters (distance-dependent low-pass)
        SVF airAbsorptionL;
        SVF airAbsorptionR;
        
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
