#ifndef THUNDERSTORM_DOMAIN_DSP_LOWPASSFILTER_H
#define THUNDERSTORM_DOMAIN_DSP_LOWPASSFILTER_H

namespace ThunderStorm::Domain::DSP {

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

}

#endif // THUNDERSTORM_DOMAIN_DSP_LOWPASSFILTER_H
