#ifndef THUNDERSTORM_DOMAIN_DSP_PINKNOISEGENERATOR_H
#define THUNDERSTORM_DOMAIN_DSP_PINKNOISEGENERATOR_H

namespace ThunderStorm::Domain::DSP {

    // 1/f Pink Noise generator using the Voss-McCartney algorithm
    class PinkNoiseGenerator {
    private:
        unsigned int randSeed;
        float rows[12];
        float runningSum;
        int index;

        float NextWhite();

    public:
        PinkNoiseGenerator(unsigned int seed);
        void Reset();
        float NextSample();
    };

}

#endif // THUNDERSTORM_DOMAIN_DSP_PINKNOISEGENERATOR_H
