#ifndef THUNDERSTORM_DOMAIN_DSP_DYNAMICSCOMPRESSOR_H
#define THUNDERSTORM_DOMAIN_DSP_DYNAMICSCOMPRESSOR_H

namespace ThunderStorm::Domain::DSP {

    // Dynamic Range Compressor
    class DynamicsCompressor {
    public:
        float thresholdDb;
        float kneeDb;
        float ratio;
        float attackSec;
        float releaseSec;
        float sampleRate;
        float currentGainDb;

        DynamicsCompressor();
        void Reset();
        void Process(float& inL, float& inR);
    };

}

#endif // THUNDERSTORM_DOMAIN_DSP_DYNAMICSCOMPRESSOR_H
