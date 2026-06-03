#include "Rumbler.h"
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

    static const EnvPoint rumblerEnvPoints[] = {
        {0.0f, 2.5f},
        {0.5f, 0.15f},
        {0.8f, 1.7f},
        {1.0f, 0.12f},
        {1.3f, 0.8f},
        {2.0f, 0.25f},
        {6.0f, 0.05f},
        {9.0f, 0.02f},
        {12.0f, 0.0f}
    };

    Rumbler::Rumbler() 
        : randSeed(0)
        , active(false)
        , delaySamples(0)
        , elapsedSamples(0)
        , panL(0.0f)
        , panR(0.0f)
        , phaserPhase(0.0f)
        , shState(0.0f)
    {}
    
    void Rumbler::Reset() {
        active = false;
        elapsedSamples = 0;
        delaySamples = 0;
        phaserPhase = 0.0f;
        shState = 0.0f;
        lp1.Reset();
        lp2.Reset();
        hpf.Reset();
    }
    
    void Rumbler::Trigger(float thunderDistance, float sampleRate, unsigned int seed) {
        randSeed = seed;
        active = true;
        elapsedSamples = 0;
        
        float rumbleDelaySec = thunderDistance / 343.0f;
        delaySamples = static_cast<int>(rumbleDelaySec * sampleRate);
        
        randSeed = randSeed * 196314165 + 907633515;
        float rp = ((randSeed % 1000) / 1000.0f);
        panL = std::sqrt(1.0f - rp);
        panR = std::sqrt(rp);
        phaserPhase = 0.0f;
        shState = 0.0f;
        lp1.Reset();
        lp2.Reset();
        hpf.Reset();
    }
    
    void Rumbler::Process(float& dryThunderL, float& dryThunderR, float sampleRate, float thunderRumble) {
        if (!active) return;
        
        if (delaySamples > 0) {
            --delaySamples;
            return;
        }
        
        float elapsedSec = static_cast<float>(elapsedSamples) / sampleRate;
        if (elapsedSec > 12.0f) {
            active = false;
            return;
        }
        
        // Fade out over last 1.5 seconds to prevent abrupt cutoff
        float fadeOut = 1.0f;
        if (elapsedSec > 10.5f) {
            fadeOut = 1.0f - (elapsedSec - 10.5f) / 1.5f;
        }
        float env = EvaluateEnvelope(elapsedSec, rumblerEnvPoints, 9) * thunderRumble * fadeOut;
        
        // Cutoff frequency ft sweeps from 1000Hz to 0Hz over 12.0s
        float fc = 1000.0f * (1.0f - elapsedSec / 12.0f);
        if (fc < 80.0f) fc = 80.0f;
        float lpCoef = fc * 2.0f * 3.14159265f / sampleRate;
        if (lpCoef > 0.95f) lpCoef = 0.95f;
        
        float wn1 = NextNoise();
        float wn2 = NextNoise();
        
        float rn1 = lp1.Process(wn1, lpCoef);
        if (rn1 < 0.0f) rn1 = 0.0f;
        
        // Phasor triggers S&H
        float prevPhase = phaserPhase;
        float phFreq = env + 1.0f;
        phaserPhase += phFreq / sampleRate;
        if (phaserPhase >= 1.0f) phaserPhase -= 1.0f;
        
        bool shTrigger = (phaserPhase < prevPhase);
        if (shTrigger) {
            shState = lp2.Process(wn2, lpCoef);
        }
        
        // Waveshaping on shState: Y = |shState * 0.40f|
        float Y = std::abs(shState * 0.40f);
        float w = 0.5f * Y * (1.0f - 4.0f * (Y - 0.5f) * (Y - 0.5f));
        
        // High-pass filter w at 300Hz, Q=9
        float hpG = 3.14159265f * 300.0f / sampleRate;
        float hpK = 1.0f / 9.0f;
        float lpOut = 0.0f, bpOut = 0.0f, hpOut = 0.0f;
        hpf.ProcessMulti(w, hpG, hpK, lpOut, bpOut, hpOut);
        
        // Mixed rumbler output is (rn1 + hpOut) * env
        float rumblerOutput = (rn1 + hpOut) * env;
        
        dryThunderL += rumblerOutput * panL;
        dryThunderR += rumblerOutput * panR;
        
        elapsedSamples++;
    }

}
