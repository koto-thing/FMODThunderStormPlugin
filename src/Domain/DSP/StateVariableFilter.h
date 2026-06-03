#ifndef THUNDERSTORM_DOMAIN_DSP_STATEVARIABLEFILTER_H
#define THUNDERSTORM_DOMAIN_DSP_STATEVARIABLEFILTER_H

namespace ThunderStorm::Domain::DSP {

    // State Variable Filter for shaping raindrop resonance and thunder components
    struct StateVariableFilter {
        float ic1eq;
        float ic2eq;
        
        StateVariableFilter() : ic1eq(0.0f), ic2eq(0.0f) {}
        
        void Reset() { 
            ic1eq = 0.0f; 
            ic2eq = 0.0f; 
        }
        
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

}

#endif // THUNDERSTORM_DOMAIN_DSP_STATEVARIABLEFILTER_H
