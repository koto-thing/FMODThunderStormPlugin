#include "DynamicsCompressor.h"
#include <algorithm>
#include <cmath>

namespace ThunderStorm::Domain::DSP {

    DynamicsCompressor::DynamicsCompressor() 
        : thresholdDb(-20.0f)
        , kneeDb(20.0f)
        , ratio(12.0f)
        , attackSec(0.0f)
        , releaseSec(0.5f)
        , sampleRate(48000.0f)
        , currentGainDb(0.0f)
    {}

    void DynamicsCompressor::Reset() {
        currentGainDb = 0.0f;
    }

    void DynamicsCompressor::Process(float& inL, float& inR) {
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

}
