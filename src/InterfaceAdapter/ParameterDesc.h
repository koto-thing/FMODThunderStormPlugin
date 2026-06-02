#ifndef FMODTHUNDERSTORMPLUGIN_PARAMETERDESC_H
#define FMODTHUNDERSTORMPLUGIN_PARAMETERDESC_H

#include "fmod.h"
#include "fmod_dsp.h"
#include "ParameterIndex.h"

namespace ThunderStorm::InterfaceAdapter {
    extern FMOD_DSP_PARAMETER_DESC* s_Params[NUM_PARAMETERS];
    
    void InitParameterDescs();
    
    FMOD_RESULT F_CALL ThunderStorm_SetParameterFloat(FMOD_DSP_STATE *dsp_state, int index, float value);
    FMOD_RESULT F_CALL ThunderStorm_SetParameterInt(FMOD_DSP_STATE *dsp_state, int index, int value);
    FMOD_RESULT F_CALL ThunderStorm_SetParameterBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL value);
    
    FMOD_RESULT F_CALL ThunderStorm_GetParameterFloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr);
    FMOD_RESULT F_CALL ThunderStorm_GetParameterInt(FMOD_DSP_STATE *dsp_state, int index, int *value, char *valuestr);
    FMOD_RESULT F_CALL ThunderStorm_GetParameterBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL *value, char *valuestr);
}

#endif // FMODTHUNDERSTORMPLUGIN_PARAMETERDESC_H
