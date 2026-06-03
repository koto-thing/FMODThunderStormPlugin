#include "ThunderStormEngine.h"
#include <algorithm>
#include <cmath>

namespace ThunderStorm::UseCase {

    void ThunderStormEngine::Reset() {
        rainPinkL.Reset();
        rainPinkR.Reset();
        rumbleLP_L.Reset(); rumbleLP_R.Reset();
        rumbleHP_L.Reset(); rumbleHP_R.Reset();
        showerLP_L.Reset(); showerLP_R.Reset();
        showerHP_L.Reset(); showerHP_R.Reset();
        
        for (int i = 0; i < 64; ++i) {
            rainDrops[i] = Domain::Rain::RainDropVoice();
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
            strikes[i].Reset();
        }
        rumbler.Reset();
        afterimage.Reset();
        deepener.Reset();

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
                            randSeed = randSeed * 196314165 + 907633515;
                            unsigned int strikeSeed = randSeed;
                            bool impulse = (j % 2 == 0);
                            strikes[j].Trigger(params.thunderStrike, params.thunderDistance, sampleRate, impulse, strikeSeed);
                        } else {
                            strikes[j].active = false;
                        }
                    }
                    
                    // 2. Rumbler setup
                    randSeed = randSeed * 196314165 + 907633515;
                    rumbler.Trigger(params.thunderDistance, sampleRate, randSeed);
                    
                    // 3. Afterimage setup
                    randSeed = randSeed * 196314165 + 907633515;
                    afterimage.Trigger(params.thunderDistance, sampleRate, randSeed);
                    
                    // 4. Deepener setup
                    randSeed = randSeed * 196314165 + 907633515;
                    deepener.Trigger(params.thunderDistance, sampleRate, randSeed);
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
                if (strikes[j].active) {
                    strikes[j].Process(dryThunderL, dryThunderR, sampleRate);
                }
            }

            // 2. Rumbler
            if (rumbler.active) {
                rumbler.Process(dryThunderL, dryThunderR, sampleRate, params.thunderRumble);
            }

            // 3. Afterimage
            if (afterimage.active) {
                afterimage.Process(dryThunderL, dryThunderR, sampleRate, params.thunderStrike);
            }

            // 4. Deepener
            if (deepener.active) {
                deepener.Process(dryThunderL, dryThunderR, sampleRate, params.thunderGrowl, params.thunderDistance);
            }

            // Calculate distance-dependent air absorption cutoff frequency (ISO 9613-1 based approximation)
            // fc = 80Hz + 19920Hz / (1.0 + alpha * D^2), where alpha = 1.678e-5
            {
                float maxFc = std::min(18000.0f, 0.45f * sampleRate);
                float fcAir = 80.0f + 19920.0f / (1.0f + 1.678e-5f * params.thunderDistance * params.thunderDistance);
                if (fcAir > maxFc) fcAir = maxFc;
                if (fcAir < 40.0f) fcAir = 40.0f;

                float gAir = std::tan(3.1415926535f * fcAir / sampleRate);
                float kAir = 1.41421356f; // Q = 0.707

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

            // Master scaling: Compensate for multi-component summing
            thunderOutL *= 0.28f;
            thunderOutR *= 0.28f;

            // Apply dynamic range compressor to the final combined thunder signal
            compressor.Process(thunderOutL, thunderOutR);

            // Combine Rain and Thunder with independent volume controls
            float finalL = rainOutL * params.rainVolume + thunderOutL * params.thunderVolume;
            float finalR = rainOutR * params.rainVolume + thunderOutR * params.thunderVolume;

            // --- Indoor/Outdoor Muffling filter ---
            outL[i] = outputMuffleL.Process(finalL, muffleLP);
            outR[i] = outputMuffleR.Process(finalR, muffleLP);
        }
    }
}
