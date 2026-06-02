#include "ThunderStormEngine.h"
#include <algorithm>
#include <cmath>

namespace ThunderStorm::UseCase {

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

    void ThunderStormEngine::Reset() {
        rainPinkL.Reset();
        rainPinkR.Reset();
        rumbleLP_L.Reset(); rumbleLP_R.Reset();
        rumbleHP_L.Reset(); rumbleHP_R.Reset();
        showerLP_L.Reset(); showerLP_R.Reset();
        showerHP_L.Reset(); showerHP_R.Reset();
        
        for (int i = 0; i < 64; ++i) {
            rainDrops[i] = RainDropVoice();
        }
        
        liquid1LP_L.Reset(); liquid1LP_R.Reset();
        liquid1HP_L.Reset(); liquid1HP_R.Reset();
        
        liquid2LP_L.Reset(); liquid2LP_R.Reset();
        liquid2HP_L.Reset(); liquid2HP_R.Reset();

        thunderPinkL.Reset();
        thunderPinkR.Reset();
        thunderDelaySamples = 0;
        thunderTriggeredPending = false;
        writeIdx = 0;
        fbFilterL = 0.0f;
        fbFilterR = 0.0f;
        for (int i = 0; i < 16384; ++i) {
            delayBufferL[i] = 0.0f;
            delayBufferR[i] = 0.0f;
        }
        for (int i = 0; i < 5; ++i) {
            strikes[i].active = false;
            for (int k = 0; k < 40; ++k) {
                strikes[i].claps[k].active = false;
                strikes[i].claps[k].filter0.Reset();
                strikes[i].claps[k].filter1.Reset();
            }
        }
        rumbler.active = false;
        rumbler.lp1.Reset();
        rumbler.lp2.Reset();
        rumbler.hpf.Reset();
        rumbler.phaserPhase = 0.0f;
        rumbler.shState = 0.0f;
        
        afterimage.active = false;
        afterimage.lp1.Reset();
        afterimage.bpf.Reset();
        
        deepener.active = false;
        deepener.lp1.Reset();
        deepener.hp1.Reset();
        deepener.lp2.Reset();

        outputMuffleL.Reset();
        outputMuffleR.Reset();
        compressor.sampleRate = sampleRate;
        compressor.Reset();
        airAbsorptionL.Reset();
        airAbsorptionR.Reset();
    }

    void ThunderStormEngine::Process(float* outL, float* outR, unsigned int length, const ThunderStormParameters& params) {
        float rainIntensity = params.rainIntensity;
        
        // --- 1. Thunder Triggering & Delay Timer ---
        if (params.thunderTrigger && !thunderTriggeredPending) {
            float delaySec = params.thunderDistance / 343.0f;
            thunderDelaySamples = static_cast<int>(delaySec * sampleRate);
            thunderTriggeredPending = true;
        }

        // --- 2. Auto-Thunder Scheduler (Poisson process simulation) ---
        bool anyStrikeActive = false;
        for (int j = 0; j < 5; ++j) {
            if (strikes[j].active) {
                anyStrikeActive = true;
                break;
            }
        }
        if (rumbler.active || afterimage.active || deepener.active) {
            anyStrikeActive = true;
        }

        if (params.thunderFreq > 0.0f && !anyStrikeActive && thunderDelaySamples <= 0 && !thunderTriggeredPending) {
            randSeed = randSeed * 196314165 + 907633515;
            float autoRoll = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
            
            float autoProbability = params.thunderFreq * 0.0000006f; 
            if (autoRoll < autoProbability) {
                randSeed = randSeed * 196314165 + 907633515;
                float randDistFactor = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                float dist = 400.0f + 2600.0f * randDistFactor; // 400m to 3000m
                
                float delaySec = dist / 343.0f;
                thunderDelaySamples = static_cast<int>(delaySec * sampleRate);
                thunderTriggeredPending = true;
            }
        }

        // Final output muffling coefficient (Low-pass) based on Indoor/Outdoor parameter
        // 0.0 (Outdoor) -> lp = 1.0 (No filter), 1.0 (Indoor) -> lp = 0.03 (~300Hz cutoff)
        float muffleLP = 1.0f - params.indoorOutdoor * 0.97f;

        // Dynamic coefficients for Liquid 1 & Liquid 2 Filters
        // LoCut (High-pass): map 0.0-1.0 to hpCoef. 0.0 -> hpCoef = 0.999f (10Hz), 1.0 -> hpCoef = 0.85f (~1.2kHz)
        float l1_hp = 0.999f - params.l1LoCut * 0.15f;
        float l2_hp = 0.999f - params.l2LoCut * 0.15f;
        
        // HiCut (Low-pass): map 0.0-1.0 to lpCoef. 0.0 -> lpCoef = 1.0f (20kHz), 1.0 -> lpCoef = 0.02f (~200Hz)
        float l1_lp = 1.0f - params.l1HiCut * 0.98f;
        float l2_lp = 1.0f - params.l2HiCut * 0.98f;

        for (unsigned int i = 0; i < length; ++i) {
            if (thunderDelaySamples > 0) {
                --thunderDelaySamples;
                if (thunderDelaySamples == 0) {
                    thunderTriggeredPending = false;
                    
                    // Clear feedback delay buffers on fresh trigger to avoid old echo artifacts
                    for (int k = 0; k < 16384; ++k) {
                        delayBufferL[k] = 0.0f;
                        delayBufferR[k] = 0.0f;
                    }
                    writeIdx = 0;
                    fbFilterL = 0.0f;
                    fbFilterR = 0.0f;
                    airAbsorptionL.Reset();
                    airAbsorptionR.Reset();

                    // 1. Multi-Strike Lightning setup (AES paper model)
                    randSeed = randSeed * 196314165 + 907633515;
                    int numStrikes = 1 + (randSeed % 5); // 1 to 5 strikes
                    
                    for (int j = 0; j < 5; ++j) {
                        if (j < numStrikes) {
                            strikes[j].active = true;
                            strikes[j].isImpulse = (j % 2 == 0); // Even j is odd strike (1, 3, 5) -> impulse
                            
                            // Calculate strike delay (distance delay + random offset of -0.75 to +0.75 seconds)
                            float distDelaySec = params.thunderDistance / 343.0f;
                            randSeed = randSeed * 196314165 + 907633515;
                            float randOffset = -0.75f + 1.5f * ((randSeed % 1000) / 1000.0f);
                            float delayTotalSec = distDelaySec + randOffset;
                            if (delayTotalSec < 0.0f) delayTotalSec = 0.0f;
                            strikes[j].delaySamples = static_cast<int>(delayTotalSec * sampleRate);
                            
                            // Initialize claps via random walk
                            float myInitial = 1.0f;
                            float myInitial2 = 1.0f;
                            float strikeDelays[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                            strikes[j].numClaps = 0;
                            
                            randSeed = randSeed * 196314165 + 907633515;
                            float splitRandom = ((randSeed % 1000) / 1000.0f) * 150.0f; // Canvas height is 150
                            
                            for (int cIdx = 0; cIdx < 20; ++cIdx) {
                                if (myInitial < 100.0f) {
                                    int strikeNumber = cIdx % 4;
                                    float strikeTime = (100.0f - myInitial) / 100.0f;
                                    
                                    float decTimeMs = 240.0f * std::pow((1.0f - strikeTime) + 0.4f, 5.0f);
                                    float delayMs = strikeDelays[strikeNumber];
                                    
                                    if (strikes[j].numClaps < 40) {
                                        ClapEvent& c = strikes[j].claps[strikes[j].numClaps++];
                                        c.active = true;
                                        c.delaySamples = static_cast<int>((delayMs / 1000.0f) * sampleRate);
                                        c.durationSamples = static_cast<int>((decTimeMs / 1000.0f) * sampleRate);
                                        c.elapsedSamples = 0;
                                        c.maxEnv = (params.thunderStrike * 2.0f) / (1.0f + params.thunderDistance * 0.0008f);
                                        float clapFreqScale = 1.0f / (1.0f + params.thunderDistance * 0.001f);
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
                                        
                                        if (strikes[j].numClaps < 40) {
                                            ClapEvent& c = strikes[j].claps[strikes[j].numClaps++];
                                            c.active = true;
                                            c.delaySamples = static_cast<int>((delayMs2 / 1000.0f) * sampleRate);
                                            c.durationSamples = static_cast<int>((decTimeMs2 / 1000.0f) * sampleRate);
                                            c.elapsedSamples = 0;
                                            c.maxEnv = (params.thunderStrike * 2.0f) / (1.0f + params.thunderDistance * 0.0008f);
                                            float clapFreqScale = 1.0f / (1.0f + params.thunderDistance * 0.001f);
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
                            if (strikes[j].isImpulse) {
                                for (int imp = 0; imp < 20; ++imp) {
                                    randSeed = randSeed * 196314165 + 907633515;
                                    strikes[j].impulseOffsets[imp] = randSeed % static_cast<int>(sampleRate);
                                }
                                std::sort(strikes[j].impulseOffsets, strikes[j].impulseOffsets + 20);
                            }
                            
                            // Pan
                            randSeed = randSeed * 196314165 + 907633515;
                            float p = ((randSeed % 1000) / 1000.0f);
                            strikes[j].panL = std::sqrt(1.0f - p);
                            strikes[j].panR = std::sqrt(p);
                            strikes[j].elapsedSamples = 0;
                        } else {
                            strikes[j].active = false;
                        }
                    }
                    
                    // 2. Rumbler setup
                    rumbler.active = true;
                    rumbler.elapsedSamples = 0;
                    
                    float rumbleDelaySec = params.thunderDistance / 343.0f;
                    rumbler.delaySamples = static_cast<int>(rumbleDelaySec * sampleRate);
                    
                    randSeed = randSeed * 196314165 + 907633515;
                    float rp = ((randSeed % 1000) / 1000.0f);
                    rumbler.panL = std::sqrt(1.0f - rp);
                    rumbler.panR = std::sqrt(rp);
                    rumbler.phaserPhase = 0.0f;
                    rumbler.shState = 0.0f;
                    rumbler.lp1.Reset();
                    rumbler.lp2.Reset();
                    rumbler.hpf.Reset();
                    
                    // 3. Afterimage setup
                    afterimage.active = true;
                    afterimage.elapsedSamples = 0;
                    
                    float afterDelaySec = params.thunderDistance / 343.0f;
                    afterimage.delaySamples = static_cast<int>(afterDelaySec * sampleRate);
                    
                    randSeed = randSeed * 196314165 + 907633515;
                    float ap = ((randSeed % 1000) / 1000.0f);
                    afterimage.panL = std::sqrt(1.0f - ap);
                    afterimage.panR = std::sqrt(ap);
                    afterimage.lp1.Reset();
                    afterimage.bpf.Reset();
                    
                    // 4. Deepener setup
                    deepener.active = true;
                    deepener.elapsedSamples = 0;
                    
                    float deepDelaySec = params.thunderDistance / 343.0f;
                    deepener.delaySamples = static_cast<int>(deepDelaySec * sampleRate);
                    
                    randSeed = randSeed * 196314165 + 907633515;
                    float dp = ((randSeed % 1000) / 1000.0f);
                    deepener.panL = std::sqrt(1.0f - dp);
                    deepener.panR = std::sqrt(dp);
                    deepener.lp1.Reset();
                    deepener.hp1.Reset();
                    deepener.lp2.Reset();
                }
            }

            // --- Rain Background (Pink Noise) ---
            float pinkL = rainPinkL.NextSample();
            float pinkR = rainPinkR.NextSample();
            
            // Rumble LPF/HPF
            float rumble_lp = 0.015f + params.rumbleHiCut * 0.685f;
            float rumble_hp = 0.999f - params.rumbleLoCut * 0.069f;
            float rumbleBgL = rumbleHP_L.Process(rumbleLP_L.Process(pinkL, rumble_lp), rumble_hp);
            float rumbleBgR = rumbleHP_R.Process(rumbleLP_R.Process(pinkR, rumble_lp), rumble_hp);
            
            // Shower LPF/HPF
            float shower_lp = 0.10f + params.showerHiCut * 0.85f;
            float shower_hp = 0.99f - params.showerLoCut * 0.39f;
            float showerBgL = showerHP_L.Process(showerLP_L.Process(pinkL, shower_lp), shower_hp);
            float showerBgR = showerHP_R.Process(showerLP_R.Process(pinkR, shower_lp), shower_hp);

            float rumbleVol = 0.28f * rainIntensity * params.rumbleLevel;
            float showerVol = 0.22f * rainIntensity * params.showerLevel;

            float rainOutL = rumbleBgL * rumbleVol + showerBgL * showerVol;
            float rainOutR = rumbleBgR * rumbleVol + showerBgR * showerVol;

            // --- Granular Raindrops (LIQUID 1 & LIQUID 2) ---
            float l1Prob = params.l1Density * 0.030f;
            randSeed = randSeed * 196314165 + 907633515;
            float roll1 = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
            if (roll1 < l1Prob) {
                for (int v = 0; v < 64; ++v) {
                    if (!rainDrops[v].active) {
                        randSeed = randSeed * 196314165 + 907633515;
                        float freqRoll = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        float fc = 1200.0f + 4500.0f * params.l1Color + 500.0f * freqRoll;
                        float Q = 3.5f + 4.5f * params.l1Color;
                        
                        float durBase = 2.0f + 10.0f * params.l1Viscosity;
                        randSeed = randSeed * 196314165 + 907633515;
                        float durRoll = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        float duration = durBase * (0.8f + 0.4f * durRoll);
                        
                        randSeed = randSeed * 196314165 + 907633515;
                        float volRoll = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        float minV = params.l1Min;
                        float maxV = params.l1Max;
                        if (minV > maxV) minV = maxV;
                        float volume = minV + (maxV - minV) * volRoll;
                        
                        randSeed = randSeed * 196314165 + 907633515;
                        float pan = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        int dropType = (freqRoll > 0.35f) ? 0 : 2;
                        
                        rainDrops[v].Trigger(0, dropType, fc, Q, duration, sampleRate, volume, pan, randSeed);
                        break;
                    }
                }
            }

            float l2Prob = params.l2Density * 0.020f;
            randSeed = randSeed * 196314165 + 907633515;
            float roll2 = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
            if (roll2 < l2Prob) {
                for (int v = 0; v < 64; ++v) {
                    if (!rainDrops[v].active) {
                        randSeed = randSeed * 196314165 + 907633515;
                        float freqRoll = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        float fc = 100.0f + 850.0f * params.l2Color + 150.0f * freqRoll;
                        float Q = 1.5f + 2.5f * params.l2Color;
                        
                        float durBase = 6.0f + 18.0f * params.l2Viscosity;
                        randSeed = randSeed * 196314165 + 907633515;
                        float durRoll = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        float duration = durBase * (0.8f + 0.4f * durRoll);
                        
                        randSeed = randSeed * 196314165 + 907633515;
                        float volRoll = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        float minV = params.l2Min;
                        float maxV = params.l2Max;
                        if (minV > maxV) minV = maxV;
                        float volume = minV + (maxV - minV) * volRoll;
                        
                        randSeed = randSeed * 196314165 + 907633515;
                        float pan = (static_cast<float>(static_cast<int>(randSeed)) / 2147483648.0f + 1.0f) * 0.5f;
                        int dropType = (freqRoll > 0.3f) ? 1 : 2;
                        
                        rainDrops[v].Trigger(1, dropType, fc, Q, duration, sampleRate, volume, pan, randSeed);
                        break;
                    }
                }
            }

            float l1OutL = 0.0f;
            float l1OutR = 0.0f;
            float l2OutL = 0.0f;
            float l2OutR = 0.0f;
            
            for (int v = 0; v < 64; ++v) {
                if (rainDrops[v].active) {
                    float outV_L = 0.0f;
                    float outV_R = 0.0f;
                    rainDrops[v].Process(outV_L, outV_R, sampleRate);
                    if (rainDrops[v].generatorId == 0) {
                        l1OutL += outV_L;
                        l1OutR += outV_R;
                    } else {
                        l2OutL += outV_L;
                        l2OutR += outV_R;
                    }
                }
            }
            
            float filtered_l1_L = liquid1HP_L.Process(liquid1LP_L.Process(l1OutL, l1_lp), l1_hp);
            float filtered_l1_R = liquid1HP_R.Process(liquid1LP_R.Process(l1OutR, l1_lp), l1_hp);
            float filtered_l2_L = liquid2HP_L.Process(liquid2LP_L.Process(l2OutL, l2_lp), l2_hp);
            float filtered_l2_R = liquid2HP_R.Process(liquid2LP_R.Process(l2OutR, l2_lp), l2_hp);
            
            rainOutL += filtered_l1_L * params.l1Level + filtered_l2_L * params.l2Level;
            rainOutR += filtered_l1_R * params.l1Level + filtered_l2_R * params.l2Level;

            // --- Thunder Sound ---
            float dryThunderL = 0.0f;
            float dryThunderR = 0.0f;

            // 1. Multi-Strike Claps
            for (int j = 0; j < 5; ++j) {
                if (!strikes[j].active) continue;
                
                if (strikes[j].delaySamples > 0) {
                    --strikes[j].delaySamples;
                    continue;
                }
                
                int elapsed = strikes[j].elapsedSamples;
                
                // Determine source signal sigX
                float sigX = 0.0f;
                if (strikes[j].isImpulse) {
                    if (elapsed < static_cast<int>(sampleRate)) {
                        int pulseWidth = static_cast<int>(0.0005f * sampleRate);
                        for (int imp = 0; imp < 20; ++imp) {
                            int offset = strikes[j].impulseOffsets[imp];
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
                
                for (int k = 0; k < strikes[j].numClaps; ++k) {
                    ClapEvent& c = strikes[j].claps[k];
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
                dryThunderL += finalStrike * strikes[j].panL;
                dryThunderR += finalStrike * strikes[j].panR;
                
                strikes[j].elapsedSamples++;
                
                if (!anyClapActive && elapsed > static_cast<int>(sampleRate)) {
                    strikes[j].active = false;
                }
            }

            // 2. Rumbler
            if (rumbler.active) {
                if (rumbler.delaySamples > 0) {
                    --rumbler.delaySamples;
                } else {
                    float elapsedSec = static_cast<float>(rumbler.elapsedSamples) / sampleRate;
                    if (elapsedSec > 12.0f) {
                        rumbler.active = false;
                    } else {
                        // Fade out over last 1.5 seconds to prevent abrupt cutoff
                        float fadeOut = 1.0f;
                        if (elapsedSec > 10.5f) fadeOut = 1.0f - (elapsedSec - 10.5f) / 1.5f;
                        float env = EvaluateEnvelope(elapsedSec, rumblerEnvPoints, 9) * params.thunderRumble * fadeOut;
                        
                        // Cutoff frequency ft sweeps from 1000Hz to 0Hz over 12.0s
                        float fc = 1000.0f * (1.0f - elapsedSec / 12.0f);
                        if (fc < 80.0f) fc = 80.0f;
                        float lpCoef = fc * 2.0f * 3.14159265f / sampleRate;
                        if (lpCoef > 0.95f) lpCoef = 0.95f;
                        
                        float wn1 = NextNoise();
                        float wn2 = NextNoise();
                        
                        float rn1 = rumbler.lp1.Process(wn1, lpCoef);
                        if (rn1 < 0.0f) rn1 = 0.0f;
                        
                        // Phasor triggers S&H (frequency depends on env + 1.0)
                        float prevPhase = rumbler.phaserPhase;
                        float phFreq = env + 1.0f;
                        rumbler.phaserPhase += phFreq / sampleRate;
                        if (rumbler.phaserPhase >= 1.0f) rumbler.phaserPhase -= 1.0f;
                        
                        bool shTrigger = (rumbler.phaserPhase < prevPhase);
                        if (shTrigger) {
                            rumbler.shState = rumbler.lp2.Process(wn2, lpCoef);
                        }
                        
                        // Waveshaping on shState: Y = |shState * 0.40f|
                        float Y = std::abs(rumbler.shState * 0.40f);
                        float w = 0.5f * Y * (1.0f - 4.0f * (Y - 0.5f) * (Y - 0.5f));
                        
                        // High-pass filter w at 300Hz, Q=9
                        float hpG = 3.14159265f * 300.0f / sampleRate;
                        float hpK = 1.0f / 9.0f;
                        float lpOut = 0.0f, bpOut = 0.0f, hpOut = 0.0f;
                        rumbler.hpf.ProcessMulti(w, hpG, hpK, lpOut, bpOut, hpOut);
                        
                        // Mixed rumbler output is (rn1 + hpOut) * env
                        float rumblerOutput = (rn1 + hpOut) * env;
                        
                        dryThunderL += rumblerOutput * rumbler.panL;
                        dryThunderR += rumblerOutput * rumbler.panR;
                        
                        rumbler.elapsedSamples++;
                    }
                }
            }

            // 3. Afterimage
            if (afterimage.active) {
                if (afterimage.delaySamples > 0) {
                    --afterimage.delaySamples;
                } else {
                    float elapsedSec = static_cast<float>(afterimage.elapsedSamples) / sampleRate;
                    if (elapsedSec > 14.0f) {
                        afterimage.active = false;
                    } else {
                        // Fade out over last 1.5 seconds to prevent abrupt cutoff
                        float fadeOut = 1.0f;
                        if (elapsedSec > 12.5f) fadeOut = 1.0f - (elapsedSec - 12.5f) / 1.5f;
                        float env = EvaluateEnvelope(elapsedSec, afterimageEnvPoints, 11) * params.thunderStrike * fadeOut;
                        
                        // Cutoff frequency ft sweeps from 33Hz to 0Hz over 14.0s
                        float fc = 33.0f * (1.0f - elapsedSec / 14.0f);
                        if (fc < 2.0f) fc = 2.0f;
                        float lpCoef = fc * 2.0f * 3.14159265f / sampleRate;
                        if (lpCoef > 0.95f) lpCoef = 0.95f;
                        
                        float wn1 = NextNoise();
                        float wn2 = NextNoise();
                        
                        float filt1 = afterimage.lp1.Process(wn1, lpCoef);
                        float x = (filt1 * 80.0f) * wn2;
                        
                        // Clip bounds
                        if (x > 1.0f) x = 1.0f;
                        if (x < -1.0f) x = -1.0f;
                        
                        // 333Hz BPF (Q=4)
                        float lpG = 3.14159265f * 333.0f / sampleRate;
                        float lpK = 1.0f / 4.0f;
                        float afterSignal = afterimage.bpf.ProcessBP(x, lpG, lpK) * env * 0.4f;
                        
                        dryThunderL += afterSignal * afterimage.panL;
                        dryThunderR += afterSignal * afterimage.panR;
                        
                        afterimage.elapsedSamples++;
                    }
                }
            }

            // 4. Deepener
            if (deepener.active) {
                if (deepener.delaySamples > 0) {
                    --deepener.delaySamples;
                } else {
                    float elapsedSec = static_cast<float>(deepener.elapsedSamples) / sampleRate;
                    if (elapsedSec > 23.0f) {
                        deepener.active = false;
                    } else {
                        // Fade out over last 1.5 seconds to prevent abrupt cutoff
                        float fadeOut = 1.0f;
                        if (elapsedSec > 21.5f) fadeOut = 1.0f - (elapsedSec - 21.5f) / 1.5f;
                        // Apply distance decay
                        float deepAtten = 1.0f / (1.0f + params.thunderDistance * 0.0002f);
                        float env = EvaluateEnvelope(elapsedSec, deepenerEnvPoints, 8) * params.thunderGrowl * deepAtten * fadeOut;
                        
                        float wnL = NextNoise();
                        
                        // Filters: LPF1 (60Hz) -> HPF1 (30Hz) -> Multiply 3.5 -> Clip -> LPF2 (80Hz)
                        float lp1_coef = 60.0f * 2.0f * 3.14159265f / sampleRate;
                        float hp1_coef = std::exp(-30.0f * 2.0f * 3.14159265f / sampleRate);
                        float lp2_coef = 80.0f * 2.0f * 3.14159265f / sampleRate;
                        
                        float x = deepener.hp1.Process(deepener.lp1.Process(wnL, lp1_coef), hp1_coef) * 3.5f;
                        
                        // Clip
                        if (x > 1.0f) x = 1.0f;
                        if (x < -1.0f) x = -1.0f;
                        
                        float deepSignal = deepener.lp2.Process(x, lp2_coef) * env;
                        
                        dryThunderL += deepSignal * deepener.panL;
                        dryThunderR += deepSignal * deepener.panR;
                        
                        deepener.elapsedSamples++;
                    }
                }
            }

            // Calculate distance-dependent air absorption cutoff frequency (ISO 9613-1 based approximation)
            // fc = 80Hz + 19920Hz / (1.0 + alpha * D^2), where alpha = 1.678e-5
            {
                float maxFc = std::min(18000.0f, 0.45f * sampleRate);
                float fcAir = 80.0f + 19920.0f / (1.0f + 1.678e-5f * params.thunderDistance * params.thunderDistance);
                if (fcAir > maxFc) fcAir = maxFc;
                if (fcAir < 40.0f) fcAir = 40.0f;

                float gAir = std::tan(3.1415926535f * fcAir / sampleRate);
                float kAir = 1.41421356f; // Q = 0.707 (Butterworth low-pass)

                float lpL = 0.0f, bpL = 0.0f, hpL = 0.0f;
                float lpR = 0.0f, bpR = 0.0f, hpR = 0.0f;
                airAbsorptionL.ProcessMulti(dryThunderL, gAir, kAir, lpL, bpL, hpL);
                airAbsorptionR.ProcessMulti(dryThunderR, gAir, kAir, lpR, bpR, hpR);
                dryThunderL = lpL;
                dryThunderR = lpR;
            }

            // Apply Nyquist Feedback Delay Lines
            // Even when all sources are silent, keep ticking the delay lines so the
            // reverb tail decays naturally instead of being cut off abruptly.
            int delayTap1 = static_cast<int>(0.073f * sampleRate) & (DELAY_BUF_SIZE - 1);
            int delayTap2 = static_cast<int>(0.139f * sampleRate) & (DELAY_BUF_SIZE - 1);
            int delayTap3 = static_cast<int>(0.227f * sampleRate) & (DELAY_BUF_SIZE - 1);

            int readIdx1 = (writeIdx - delayTap1 + DELAY_BUF_SIZE) & (DELAY_BUF_SIZE - 1);
            int readIdx2 = (writeIdx - delayTap2 + DELAY_BUF_SIZE) & (DELAY_BUF_SIZE - 1);
            int readIdx3 = (writeIdx - delayTap3 + DELAY_BUF_SIZE) & (DELAY_BUF_SIZE - 1);

            float delSignalL = delayBufferL[readIdx1] * 0.40f + delayBufferL[readIdx2] * 0.30f + delayBufferL[readIdx3] * 0.20f;
            float delSignalR = delayBufferR[readIdx1] * 0.40f + delayBufferR[readIdx2] * 0.30f + delayBufferR[readIdx3] * 0.20f;

            // Feedback path LPF
            float fbLP = 0.015f + 0.025f / (1.0f + params.thunderDistance * 0.0006f);
            
            fbFilterL += fbLP * (delSignalL - fbFilterL);
            fbFilterR += fbLP * (delSignalR - fbFilterR);

            // Mix dry strike with filtered feedback reflections
            float thunderOutL = dryThunderL + fbFilterL * 0.65f;
            float thunderOutR = dryThunderR + fbFilterR * 0.65f;

            // Feed mixed signals back to the delay lines
            // Write 0 once all sources are silent so the tail decays via feedback
            delayBufferL[writeIdx] = dryThunderL + fbFilterL * 0.55f;
            delayBufferR[writeIdx] = dryThunderR + fbFilterR * 0.55f;
            writeIdx = (writeIdx + 1) & (DELAY_BUF_SIZE - 1);

            // After all thunder sources stop, keep fbFilter active as long as the tail
            // is still audible (above -80 dB ~= 0.0001) so we never hard-cut the reverb.
            bool anyThunderActive = rumbler.active || afterimage.active || deepener.active;
            for (int j2 = 0; j2 < 5; ++j2) { if (strikes[j2].active) { anyThunderActive = true; break; } }
            if (!anyThunderActive && std::abs(fbFilterL) < 0.0001f && std::abs(fbFilterR) < 0.0001f) {
                fbFilterL = 0.0f;
                fbFilterR = 0.0f;
            }

            // Master scaling: Compensate for multi-component summing (0.28f factor)
            thunderOutL *= 0.28f;
            thunderOutR *= 0.28f;

            // Apply dynamic range compressor to the final combined thunder signal
            compressor.Process(thunderOutL, thunderOutR);

            // Combine Rain and Thunder with independent volume controls
            float finalL = rainOutL * params.rainVolume + thunderOutL * params.thunderVolume;
            float finalR = rainOutR * params.rainVolume + thunderOutR * params.thunderVolume;

            // --- Indoor/Outdoor Muffling filter (Low-pass only) ---
            outL[i] = outputMuffleL.Process(finalL, muffleLP);
            outR[i] = outputMuffleR.Process(finalR, muffleLP);
        }
    }
}
