#include "DSPDesc.h"
#include "DSPCallbacks.h"
#include "../InterfaceAdapter/ParameterDesc.h"
#include "../InterfaceAdapter/ParameterIndex.h"

namespace ThunderStorm::Framework {
    
    FMOD_DSP_DESCRIPTION thunderStormDesc = {
        FMOD_PLUGIN_SDK_VERSION,
        "ThunderStorm",
        0x00010000,
        0, // 0 input buffers (FMOD PluginInstrument/Generator)
        1, // 1 output buffer
        ThunderStorm_Create,
        ThunderStorm_Release,
        ThunderStorm_Reset,
        nullptr,
        ThunderStorm_Process,
        nullptr,
        InterfaceAdapter::NUM_PARAMETERS,
        InterfaceAdapter::s_Params,
        InterfaceAdapter::ThunderStorm_SetParameterFloat,
        InterfaceAdapter::ThunderStorm_SetParameterInt,
        InterfaceAdapter::ThunderStorm_SetParameterBool,
        nullptr,
        InterfaceAdapter::ThunderStorm_GetParameterFloat,
        InterfaceAdapter::ThunderStorm_GetParameterInt,
        InterfaceAdapter::ThunderStorm_GetParameterBool,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };
}

extern "C" {
    F_EXPORT FMOD_DSP_DESCRIPTION* FMODGetDSPDescription() {
        ThunderStorm::InterfaceAdapter::InitParameterDescs();
        return &ThunderStorm::Framework::thunderStormDesc;
    }
}