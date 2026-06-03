#include "LightningStrike.h"
#include <cmath>
#include <algorithm>

namespace ThunderStorm::Domain::Thunder {

    LightningStrike::LightningStrike() 
        : randSeed(0)
        , active(false)
        , delaySamples(0)
        , panL(0.0f)
        , panR(0.0f)
        , isImpulse(false)
        , numClaps(0)
        , elapsedSamples(0)
    {
        for (int k = 0; k < 40; ++k) {
            claps[k].active = false;
        }
        for (int i = 0; i < 20; ++i) {
            impulseOffsets[i] = 0;
        }
    }
    
    void LightningStrike::Reset() {
        active = false;
        elapsedSamples = 0;
        delaySamples = 0;
        for (int k = 0; k < 40; ++k) {
            claps[k].active = false;
            claps[k].filter0.Reset();
            claps[k].filter1.Reset();
        }
    }
    
    void LightningStrike::Trigger(float thunderStrike, float thunderDistance, float sampleRate, bool impulse, unsigned int seed) {
        randSeed = seed;
        isImpulse = impulse;
        
        // Calculate strike delay
        float distDelaySec = thunderDistance / 343.0f;
        randSeed = randSeed * 196314165 + 907633515;
        float randOffset = -0.75f + 1.5f * ((randSeed % 1000) / 1000.0f);
        float delayTotalSec = distDelaySec + randOffset;
        if (delayTotalSec < 0.0f) delayTotalSec = 0.0f;
        delaySamples = static_cast<int>(delayTotalSec * sampleRate);
        
        // Initialize claps via random walk
        float myInitial = 1.0f;
        float myInitial2 = 1.0f;
        float strikeDelays[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        numClaps = 0;
        
        randSeed = randSeed * 196314165 + 907633515;
        float splitRandom = ((randSeed % 1000) / 1000.0f) * 150.0f; // Canvas height is 150
        
        for (int cIdx = 0; cIdx < 20; ++cIdx) {
            if (myInitial < 100.0f) {
                int strikeNumber = cIdx % 4;
                float strikeTime = (100.0f - myInitial) / 100.0f;
                
                float decTimeMs = 240.0f * std::pow((1.0f - strikeTime) + 0.4f, 5.0f);
                float delayMs = strikeDelays[strikeNumber];
                
                if (numClaps < 40) {
                    ClapEvent& c = claps[numClaps++];
                    c.active = true;
                    c.delaySamples = static_cast<int>((delayMs / 1000.0f) * sampleRate);
                    c.durationSamples = static_cast<int>((decTimeMs / 1000.0f) * sampleRate);
                    c.elapsedSamples = 0;
                    c.maxEnv = (thunderStrike * 2.0f) / (1.0f + thunderDistance * 0.0008f);
                    float clapFreqScale = 1.0f / (1.0f + thunderDistance * 0.001f);
                    c.fcStart = (strikeTime * 1200.0f + 80.0f) * clapFreqScale;
                    c.filter0.Reset();
                    c.filter1.Reset();
                }
                
                strikeDelays[strikeNumber] += decTimeMs;
                
                bool split = false;
                if (myInitial2 >= splitRandom) {
                    split = true;
                    int strikeNumber2 = (cIdx + 2) % 4;
                    float strikeTime2 = (100.0f - myInitial2) / 100.0f;
                    
                    float decTimeMs2 = 240.0f * std::pow((1.0f - strikeTime2) + 0.4f, 5.0f);
                    float delayMs2 = strikeDelays[strikeNumber2];
                    
                    if (numClaps < 40) {
                        ClapEvent& c = claps[numClaps++];
                        c.active = true;
                        c.delaySamples = static_cast<int>((delayMs2 / 1000.0f) * sampleRate);
                        c.durationSamples = static_cast<int>((decTimeMs2 / 1000.0f) * sampleRate);
                        c.elapsedSamples = 0;
                        c.maxEnv = (thunderStrike * 2.0f) / (1.0f + thunderDistance * 0.0008f);
                        float clapFreqScale = 1.0f / (1.0f + thunderDistance * 0.001f);
                        c.fcStart = (strikeTime2 * 1200.0f + 80.0f) * clapFreqScale;
                        c.filter0.Reset();
                        c.filter1.Reset();
                    }
                    
                    strikeDelays[strikeNumber2] += decTimeMs2;
                }
                
                randSeed = randSeed * 196314165 + 907633515;
                float rVal = ((randSeed % 1000) / 1000.0f) * 10.0f;
                myInitial += rVal;
                if (split) {
                    randSeed = randSeed * 196314165 + 907633515;
                    float rVal2 = ((randSeed % 1000) / 1000.0f) * 10.0f;
                    myInitial2 += rVal2;
                } else {
                    myInitial2 += rVal;
                }
            }
        }
        
        // Initialize impulses if needed
        if (isImpulse) {
            for (int imp = 0; imp < 20; ++imp) {
                randSeed = randSeed * 196314165 + 907633515;
                impulseOffsets[imp] = randSeed % static_cast<int>(sampleRate);
            }
            std::sort(impulseOffsets, impulseOffsets + 20);
        }
        
        // Pan
        randSeed = randSeed * 196314165 + 907633515;
        float p = ((randSeed % 1000) / 1000.0f);
        panL = std::sqrt(1.0f - p);
        panR = std::sqrt(p);
        elapsedSamples = 0;
        active = true;
    }
    
    void LightningStrike::Process(float& dryThunderL, float& dryThunderR, float sampleRate) {
        if (!active) return;
        
        if (delaySamples > 0) {
            --delaySamples;
            return;
        }
        
        int elapsed = elapsedSamples;
        
        // Determine source signal sigX
        float sigX = 0.0f;
        if (isImpulse) {
            if (elapsed < static_cast<int>(sampleRate)) {
                int pulseWidth = static_cast<int>(0.0005f * sampleRate);
                for (int imp = 0; imp < 20; ++imp) {
                    int offset = impulseOffsets[imp];
                    if (elapsed >= offset && elapsed < offset + pulseWidth) {
                        sigX = 1.0f;
                        break;
                    }
                }
            }
        } else {
            sigX = NextNoise();
        }
        
        float strikeOutput = 0.0f;
        bool anyClapActive = false;
        
        for (int k = 0; k < numClaps; ++k) {
            ClapEvent& c = claps[k];
            if (!c.active) continue;
            
            if (elapsed >= c.delaySamples) {
                anyClapActive = true;
                
                float tRatio = static_cast<float>(c.elapsedSamples) / c.durationSamples;
                if (tRatio > 1.0f) tRatio = 1.0f;
                
                // Linear decay for envelope
                float gain = c.maxEnv * (1.0f - tRatio);
                if (gain < 0.0f) gain = 0.0f;
                
                // Linear sweep down for fc: fcStart -> fcStart * 0.5
                float fc = c.fcStart * (1.0f - 0.5f * tRatio);
                if (fc < 50.0f) fc = 50.0f;
                
                float lpG0 = 3.14159265f * fc / sampleRate;
                float lpK0 = 1.0f / 7.0f; // Q = 7
                float val0 = c.filter0.ProcessBP(sigX, lpG0, lpK0);
                
                float lpG1 = 3.14159265f * (fc * 0.5f) / sampleRate;
                float lpK1 = 1.0f / 7.0f; // Q = 7
                float val1 = c.filter1.ProcessBP(sigX, lpG1, lpK1);
                
                strikeOutput += (val0 + val1) * gain;
                
                c.elapsedSamples++;
                if (c.elapsedSamples >= c.durationSamples) {
                    c.active = false;
                }
            } else {
                // Clap is not triggered yet but it is scheduled
                anyClapActive = true;
            }
        }
        
        // Mix mono strikeOutput panned to L/R
        float finalStrike = strikeOutput * 2.0f; // Base scaling
        dryThunderL += finalStrike * panL;
        dryThunderR += finalStrike * panR;
        
        elapsedSamples++;
        
        if (!anyClapActive && elapsed > static_cast<int>(sampleRate)) {
            active = false;
        }
    }

}
