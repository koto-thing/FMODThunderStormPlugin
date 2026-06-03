#include "Afterimage.h"
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

    static const EnvPoint afterimageEnvPoints[] = {
        {0.0f, 2.0f},
        {0.3f, 0.4f},
        {0.8f, 1.5f},
        {1.2f, 0.25f},
        {2.0f, 1.25f},
        {3.0f, 0.6f},
        {5.0f, 1.15f},
        {7.0f, 0.35f},
        {9.5f, 0.15f},
        {12.5f, 0.04f},
        {14.0f, 0.0f}
    };

    Afterimage::Afterimage() 
        : randSeed(0)
        , active(false)
        , delaySamples(0)
        , elapsedSamples(0)
        , panL(0.0f)
        , panR(0.0f)
    {}
    
    void Afterimage::Reset() {
        active = false;
        elapsedSamples = 0;
        delaySamples = 0;
        lp1.Reset();
        bpf.Reset();
    }
    
    void Afterimage::Trigger(float thunderDistance, float sampleRate, unsigned int seed) {
        randSeed = seed;
        active = true;
        elapsedSamples = 0;
        
        float afterDelaySec = thunderDistance / 343.0f;
        delaySamples = static_cast<int>(afterDelaySec * sampleRate);
        
        randSeed = randSeed * 196314165 + 907633515;
        float ap = ((randSeed % 1000) / 1000.0f);
        panL = std::sqrt(1.0f - ap);
        panR = std::sqrt(ap);
        lp1.Reset();
        bpf.Reset();
    }
    
    void Afterimage::Process(float& dryThunderL, float& dryThunderR, float sampleRate, float thunderStrike) {
        if (!active) return;
        
        if (delaySamples > 0) {
            --delaySamples;
            return;
        }
        
        float elapsedSec = static_cast<float>(elapsedSamples) / sampleRate;
        if (elapsedSec > 14.0f) {
            active = false;
            return;
        }
        
        // Fade out over last 1.5 seconds to prevent abrupt cutoff
        float fadeOut = 1.0f;
        if (elapsedSec > 12.5f) {
            fadeOut = 1.0f - (elapsedSec - 12.5f) / 1.5f;
        }
        float env = EvaluateEnvelope(elapsedSec, afterimageEnvPoints, 11) * thunderStrike * fadeOut;
        
        // Cutoff frequency ft sweeps from 33Hz to 0Hz over 14.0s
        float fc = 33.0f * (1.0f - elapsedSec / 14.0f);
        if (fc < 2.0f) fc = 2.0f;
        float lpCoef = fc * 2.0f * 3.14159265f / sampleRate;
        if (lpCoef > 0.95f) lpCoef = 0.95f;
        
        float wn1 = NextNoise();
        float wn2 = NextNoise();
        
        float filt1 = lp1.Process(wn1, lpCoef);
        float x = (filt1 * 80.0f) * wn2;
        
        // Clip bounds
        if (x > 1.0f) x = 1.0f;
        if (x < -1.0f) x = -1.0f;
        
        // 333Hz BPF
        float lpG = 3.14159265f * 333.0f / sampleRate;
        float lpK = 1.0f / 4.0f;
        float afterSignal = bpf.ProcessBP(x, lpG, lpK) * env * 0.4f;
        
        dryThunderL += afterSignal * panL;
        dryThunderR += afterSignal * panR;
        
        elapsedSamples++;
    }

}
