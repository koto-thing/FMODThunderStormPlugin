#ifndef THUNDERSTORM_DOMAIN_DSP_HIGHPASSFILTER_H
#define THUNDERSTORM_DOMAIN_DSP_HIGHPASSFILTER_H

namespace ThunderStorm::Domain::DSP {

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

}

#endif // THUNDERSTORM_DOMAIN_DSP_HIGHPASSFILTER_H
