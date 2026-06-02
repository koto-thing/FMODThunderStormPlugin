#ifndef FMODTHUNDERSTORMPLUGIN_THUNDERSTORMPARAMETERS_H
#define FMODTHUNDERSTORMPLUGIN_THUNDERSTORMPARAMETERS_H

namespace ThunderStorm::UseCase {
    struct ThunderStormParameters {
        float rainIntensity = 0.5f;
        bool thunderTrigger = false;
        float thunderDistance = 1000.0f;
        float thunderFreq = 0.0f;
        float indoorOutdoor = 0.0f;
        float rainVolume = 0.8f;
        float thunderVolume = 0.8f;
        float thunderStrike = 0.9f;
        float thunderRumble = 0.5f;
        float thunderGrowl = 0.5f;
        
        // Liquid 1 parameters
        float l1Level = 0.5f;
        float l1Density = 0.4f;
        float l1Color = 0.5f;
        float l1Viscosity = 0.3f;
        float l1Min = 0.01f;
        float l1Max = 0.08f;
        float l1LoCut = 0.2f;
        float l1HiCut = 0.2f;
        
        // Liquid 2 parameters
        float l2Level = 0.5f;
        float l2Density = 0.2f;
        float l2Color = 0.3f;
        float l2Viscosity = 0.5f;
        float l2Min = 0.01f;
        float l2Max = 0.06f;
        float l2LoCut = 0.1f;
        float l2HiCut = 0.3f;

        // Rumble parameters (background low-frequency rain)
        float rumbleLevel = 0.5f;
        float rumbleLoCut = 0.1f;
        float rumbleHiCut = 0.1f;
        
        // Shower parameters (background high-frequency rain)
        float showerLevel = 0.5f;
        float showerLoCut = 0.2f;
        float showerHiCut = 0.2f;
    };
}

#endif // FMODTHUNDERSTORMPLUGIN_THUNDERSTORMPARAMETERS_H