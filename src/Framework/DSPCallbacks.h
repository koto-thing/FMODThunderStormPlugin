#ifndef FMODTHUNDERSTORMPLUGIN
#define FMODTHUNDERSTORMPLUGIN

#include "fmod.h"
#include "fmod_dsp.h"

namespace ThunderStorm::Framework {
    FMOD_RESULT F_CALL ThunderStorm_Create(FMOD_DSP_STATE *dspState);
    FMOD_RESULT F_CALL ThunderStorm_Release(FMOD_DSP_STATE *dsp_state);
    FMOD_RESULT F_CALL ThunderStorm_Reset(FMOD_DSP_STATE *dsp_State);
    FMOD_RESULT F_CALL ThunderStorm_Process(FMOD_DSP_STATE *dsp_state, unsigned int length,
        const FMOD_DSP_BUFFER_ARRAY *inBuffers, FMOD_DSP_BUFFER_ARRAY *outBuffers,
        FMOD_BOOL inputsIdle, FMOD_DSP_PROCESS_OPERATION op);
}

#endif // FMODTHUNDERSTORMPLUGIN