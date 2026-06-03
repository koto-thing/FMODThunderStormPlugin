#include "Deepener.h"
#include <cmath>
#include <algorithm>

namespace ThunderStorm::Domain::Thunder {

    struct EnvPoint {
        float timeSec;
        float val;
    };

    static float EvaluateEnvelope(float elapsedSec, const EnvPoint* points, int numPoints) {
        if (elapsedSec <= points[0].timeSec) {
            return points[0].val;
        }
        if (elapsedSec >= points[numPoints - 1].timeSec) {
            return points[numPoints - 1].val;
        }
        for (int i = 0; i < numPoints - 1; ++i) {
            if (elapsedSec >= points[i].timeSec && elapsedSec <= points[i + 1].timeSec) {
                float t0 = points[i].timeSec;
                float t1 = points[i + 1].timeSec;
                float v0 = points[i].val;
                float v1 = points[i + 1].val;
                float t = (elapsedSec - t0) / (t1 - t0);
                return v0 + t * (v1 - v0);
            }
        }
        return 0.0f;
    }

    static const EnvPoint deepenerEnvPoints[] = {
        {0.0f, 6.0f},
        {2.0f, 1.75f},
        {3.0f, 5.0f},
        {6.0f, 1.5f},
        {7.0f, 4.15f},
        {18.0f, 0.08f},
        {21.5f, 0.02f},
        {23.0f, 0.0f}
    };

    Deepener::Deepener() 
        : randSeed(0)
        , active(false)
        , delaySamples(0)
        , elapsedSamples(0)
        , panL(0.0f)
        , panR(0.0f)
    {}
    
    void Deepener::Reset() {
        active = false;
        elapsedSamples = 0;
        delaySamples = 0;
        lp1.Reset();
        hp1.Reset();
        lp2.Reset();
    }
    
    void Deepener::Trigger(float thunderDistance, float sampleRate, unsigned int seed) {
        randSeed = seed;
        active = true;
        elapsedSamples = 0;
        
        float deepDelaySec = thunderDistance / 343.0f;
        delaySamples = static_cast<int>(deepDelaySec * sampleRate);
        
        randSeed = randSeed * 196314165 + 907633515;
        float dp = ((randSeed % 1000) / 1000.0f);
        panL = std::sqrt(1.0f - dp);
        panR = std::sqrt(dp);
        lp1.Reset();
        hp1.Reset();
        lp2.Reset();
    }
    
    void Deepener::Process(float& dryThunderL, float& dryThunderR, float sampleRate, float thunderGrowl, float thunderDistance) {
        if (!active) return;
        
        if (delaySamples > 0) {
            --delaySamples;
            return;
        }
        
        float elapsedSec = static_cast<float>(elapsedSamples) / sampleRate;
        if (elapsedSec > 23.0f) {
            active = false;
            return;
        }
        
        // Fade out over last 1.5 seconds to prevent abrupt cutoff
        float fadeOut = 1.0f;
        if (elapsedSec > 21.5f) {
            fadeOut = 1.0f - (elapsedSec - 21.5f) / 1.5f;
        }
        // Apply distance decay
        float deepAtten = 1.0f / (1.0f + thunderDistance * 0.0002f);
        float env = EvaluateEnvelope(elapsedSec, deepenerEnvPoints, 8) * thunderGrowl * deepAtten * fadeOut;
        
        float wnL = NextNoise();
        
        // Filters: LPF1 (60Hz) -> HPF1 (30Hz) -> Multiply 3.5 -> Clip -> LPF2 (80Hz)
        float lp1_coef = 60.0f * 2.0f * 3.14159265f / sampleRate;
        float hp1_coef = std::exp(-30.0f * 2.0f * 3.14159265f / sampleRate);
        float lp2_coef = 80.0f * 2.0f * 3.14159265f / sampleRate;
        
        float x = hp1.Process(lp1.Process(wnL, lp1_coef), hp1_coef) * 3.5f;
        
        // Clip
        if (x > 1.0f) x = 1.0f;
        if (x < -1.0f) x = -1.0f;
        
        float deepSignal = lp2.Process(x, lp2_coef) * env;
        
        dryThunderL += deepSignal * panL;
        dryThunderR += deepSignal * panR;
        
        elapsedSamples++;
    }

}
