#define _CRT_SECURE_NO_WARNINGS
#include "ParameterDesc.h"
#include "../Framework/ThunderStormInternalData.h"

#include <cstring>
#include <cstdio>

namespace ThunderStorm::InterfaceAdapter {
    FMOD_DSP_PARAMETER_DESC* s_Params[NUM_PARAMETERS];
    
    static FMOD_DSP_PARAMETER_DESC p_RainIntensity;
    static FMOD_DSP_PARAMETER_DESC p_ThunderTrigger;
    static FMOD_DSP_PARAMETER_DESC p_ThunderDistance;
    static FMOD_DSP_PARAMETER_DESC p_ThunderFreq;
    static FMOD_DSP_PARAMETER_DESC p_IndoorOutdoor;
    static FMOD_DSP_PARAMETER_DESC p_RainVolume;
    static FMOD_DSP_PARAMETER_DESC p_ThunderVolume;
    static FMOD_DSP_PARAMETER_DESC p_ThunderStrike;
    static FMOD_DSP_PARAMETER_DESC p_ThunderRumble;
    static FMOD_DSP_PARAMETER_DESC p_ThunderGrowl;
    
    // Liquid 1
    static FMOD_DSP_PARAMETER_DESC p_L1Level;
    static FMOD_DSP_PARAMETER_DESC p_L1Density;
    static FMOD_DSP_PARAMETER_DESC p_L1Color;
    static FMOD_DSP_PARAMETER_DESC p_L1Viscosity;
    static FMOD_DSP_PARAMETER_DESC p_L1Min;
    static FMOD_DSP_PARAMETER_DESC p_L1Max;
    static FMOD_DSP_PARAMETER_DESC p_L1LoCut;
    static FMOD_DSP_PARAMETER_DESC p_L1HiCut;
    
    // Liquid 2
    static FMOD_DSP_PARAMETER_DESC p_L2Level;
    static FMOD_DSP_PARAMETER_DESC p_L2Density;
    static FMOD_DSP_PARAMETER_DESC p_L2Color;
    static FMOD_DSP_PARAMETER_DESC p_L2Viscosity;
    static FMOD_DSP_PARAMETER_DESC p_L2Min;
    static FMOD_DSP_PARAMETER_DESC p_L2Max;
    static FMOD_DSP_PARAMETER_DESC p_L2LoCut;
    static FMOD_DSP_PARAMETER_DESC p_L2HiCut;

    // Rumble
    static FMOD_DSP_PARAMETER_DESC p_RumbleLevel;
    static FMOD_DSP_PARAMETER_DESC p_RumbleLoCut;
    static FMOD_DSP_PARAMETER_DESC p_RumbleHiCut;

    // Shower
    static FMOD_DSP_PARAMETER_DESC p_ShowerLevel;
    static FMOD_DSP_PARAMETER_DESC p_ShowerLoCut;
    static FMOD_DSP_PARAMETER_DESC p_ShowerHiCut;
    
    void InitParameterDescs() {
        static bool initialized = false;
        if (initialized)
            return;
        
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_RainIntensity, "Intensity", "", "Rain Intensity", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_BOOL(p_ThunderTrigger, "Thunder", "", "Trigger Thunder", false, nullptr);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ThunderDistance, "Distance", "m", "Thunder Distance", 0.0f, 5000.0f, 1000.0f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ThunderFreq, "Auto Thunder", "", "Auto Thunder Frequency", 0.0f, 1.0f, 0.0f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_IndoorOutdoor, "Indoor/Outdoor", "", "Indoor Muffleness Amount", 0.0f, 1.0f, 0.0f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_RainVolume, "Rain Volume", "x", "Rain Final Volume Balance", 0.0f, 3.0f, 0.8f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ThunderVolume, "Thunder Volume", "x", "Thunder Final Volume Balance", 0.0f, 6.0f, 0.8f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ThunderStrike, "Thunder Strike", "", "Thunder Strike Level", 0.0f, 1.0f, 0.9f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ThunderRumble, "Thunder Rumble", "", "Thunder Rumble Level", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ThunderGrowl, "Thunder Growl", "", "Thunder Growl Level", 0.0f, 1.0f, 0.5f);
        
        // L1
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1Level, "L1 Level", "", "Liquid 1 Volume Level", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1Density, "L1 Density", "", "Liquid 1 Density", 0.0f, 1.0f, 0.4f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1Color, "L1 Color", "", "Liquid 1 Color / Frequency", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1Viscosity, "L1 Viscosity", "", "Liquid 1 Viscosity / Decay", 0.0f, 1.0f, 0.3f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1Min, "L1 Min Vol", "", "Liquid 1 Minimum Volume", 0.001f, 0.5f, 0.01f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1Max, "L1 Max Vol", "", "Liquid 1 Maximum Volume", 0.001f, 1.0f, 0.08f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1LoCut, "L1 LoCut", "Hz", "Liquid 1 Low Cut filter", 0.0f, 1.0f, 0.2f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L1HiCut, "L1 HiCut", "Hz", "Liquid 1 High Cut filter", 0.0f, 1.0f, 0.2f);
        
        // L2
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2Level, "L2 Level", "", "Liquid 2 Volume Level", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2Density, "L2 Density", "", "Liquid 2 Density", 0.0f, 1.0f, 0.2f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2Color, "L2 Color", "", "Liquid 2 Color / Frequency", 0.0f, 1.0f, 0.3f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2Viscosity, "L2 Viscosity", "", "Liquid 2 Viscosity / Decay", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2Min, "L2 Min Vol", "", "Liquid 2 Minimum Volume", 0.001f, 0.5f, 0.01f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2Max, "L2 Max Vol", "", "Liquid 2 Maximum Volume", 0.001f, 1.0f, 0.06f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2LoCut, "L2 LoCut", "Hz", "Liquid 2 Low Cut filter", 0.0f, 1.0f, 0.1f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_L2HiCut, "L2 HiCut", "Hz", "Liquid 2 High Cut filter", 0.0f, 1.0f, 0.3f);
 
        // Rumble
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_RumbleLevel, "Rumble Level", "", "Rain Background Rumble Level", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_RumbleLoCut, "Rumble LoCut", "Hz", "Rumble Low Cut filter", 0.0f, 1.0f, 0.1f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_RumbleHiCut, "Rumble HiCut", "Hz", "Rumble High Cut filter", 0.0f, 1.0f, 0.1f);
 
        // Shower
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ShowerLevel, "Shower Level", "", "Rain Background Shower Level", 0.0f, 1.0f, 0.5f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ShowerLoCut, "Shower LoCut", "Hz", "Shower Low Cut filter", 0.0f, 1.0f, 0.2f);
        FMOD_DSP_INIT_PARAMDESC_FLOAT(p_ShowerHiCut, "Shower HiCut", "Hz", "Shower High Cut filter", 0.0f, 1.0f, 0.2f);
        
        s_Params[PARAM_RAIN_INTENSITY] = &p_RainIntensity;
        s_Params[PARAM_THUNDER_TRIGGER] = &p_ThunderTrigger;
        s_Params[PARAM_THUNDER_DISTANCE] = &p_ThunderDistance;
        s_Params[PARAM_THUNDER_FREQ] = &p_ThunderFreq;
        s_Params[PARAM_INDOOR_OUTDOOR] = &p_IndoorOutdoor;
        s_Params[PARAM_RAIN_VOLUME] = &p_RainVolume;
        s_Params[PARAM_THUNDER_VOLUME] = &p_ThunderVolume;
        s_Params[PARAM_THUNDER_STRIKE] = &p_ThunderStrike;
        s_Params[PARAM_THUNDER_RUMBLE] = &p_ThunderRumble;
        s_Params[PARAM_THUNDER_GROWL] = &p_ThunderGrowl;
        
        s_Params[PARAM_L1_LEVEL] = &p_L1Level;
        s_Params[PARAM_L1_DENSITY] = &p_L1Density;
        s_Params[PARAM_L1_COLOR] = &p_L1Color;
        s_Params[PARAM_L1_VISCOSITY] = &p_L1Viscosity;
        s_Params[PARAM_L1_MIN] = &p_L1Min;
        s_Params[PARAM_L1_MAX] = &p_L1Max;
        s_Params[PARAM_L1_LOCUT] = &p_L1LoCut;
        s_Params[PARAM_L1_HICUT] = &p_L1HiCut;
        
        s_Params[PARAM_L2_LEVEL] = &p_L2Level;
        s_Params[PARAM_L2_DENSITY] = &p_L2Density;
        s_Params[PARAM_L2_COLOR] = &p_L2Color;
        s_Params[PARAM_L2_VISCOSITY] = &p_L2Viscosity;
        s_Params[PARAM_L2_MIN] = &p_L2Min;
        s_Params[PARAM_L2_MAX] = &p_L2Max;
        s_Params[PARAM_L2_LOCUT] = &p_L2LoCut;
        s_Params[PARAM_L2_HICUT] = &p_L2HiCut;

        s_Params[PARAM_RUMBLE_LEVEL] = &p_RumbleLevel;
        s_Params[PARAM_RUMBLE_LOCUT] = &p_RumbleLoCut;
        s_Params[PARAM_RUMBLE_HICUT] = &p_RumbleHiCut;

        s_Params[PARAM_SHOWER_LEVEL] = &p_ShowerLevel;
        s_Params[PARAM_SHOWER_LOCUT] = &p_ShowerLoCut;
        s_Params[PARAM_SHOWER_HICUT] = &p_ShowerHiCut;
        
        initialized = true;
    }

    FMOD_RESULT F_CALL ThunderStorm_SetParameterFloat(FMOD_DSP_STATE *dsp_state, int index, float value) {
        auto *state = static_cast<Framework::ThunderStormState*>(dsp_state->plugindata);
        if (!state) return FMOD_ERR_INVALID_PARAM;
        
        switch (index) {
            case PARAM_RAIN_INTENSITY:
                state->params.rainIntensity = value;
                return FMOD_OK;
            case PARAM_THUNDER_DISTANCE:
                state->params.thunderDistance = value;
                return FMOD_OK;
            case PARAM_THUNDER_FREQ:
                state->params.thunderFreq = value;
                return FMOD_OK;
            case PARAM_INDOOR_OUTDOOR:
                state->params.indoorOutdoor = value;
                return FMOD_OK;
            case PARAM_RAIN_VOLUME:
                state->params.rainVolume = value;
                return FMOD_OK;
            case PARAM_THUNDER_VOLUME:
                state->params.thunderVolume = value;
                return FMOD_OK;
            case PARAM_THUNDER_STRIKE:
                state->params.thunderStrike = value;
                return FMOD_OK;
            case PARAM_THUNDER_RUMBLE:
                state->params.thunderRumble = value;
                return FMOD_OK;
            case PARAM_THUNDER_GROWL:
                state->params.thunderGrowl = value;
                return FMOD_OK;
                
            case PARAM_L1_LEVEL:
                state->params.l1Level = value;
                return FMOD_OK;
            case PARAM_L1_DENSITY:
                state->params.l1Density = value;
                return FMOD_OK;
            case PARAM_L1_COLOR:
                state->params.l1Color = value;
                return FMOD_OK;
            case PARAM_L1_VISCOSITY:
                state->params.l1Viscosity = value;
                return FMOD_OK;
            case PARAM_L1_MIN:
                state->params.l1Min = value;
                return FMOD_OK;
            case PARAM_L1_MAX:
                state->params.l1Max = value;
                return FMOD_OK;
            case PARAM_L1_LOCUT:
                state->params.l1LoCut = value;
                return FMOD_OK;
            case PARAM_L1_HICUT:
                state->params.l1HiCut = value;
                return FMOD_OK;
                
            case PARAM_L2_LEVEL:
                state->params.l2Level = value;
                return FMOD_OK;
            case PARAM_L2_DENSITY:
                state->params.l2Density = value;
                return FMOD_OK;
            case PARAM_L2_COLOR:
                state->params.l2Color = value;
                return FMOD_OK;
            case PARAM_L2_VISCOSITY:
                state->params.l2Viscosity = value;
                return FMOD_OK;
            case PARAM_L2_MIN:
                state->params.l2Min = value;
                return FMOD_OK;
            case PARAM_L2_MAX:
                state->params.l2Max = value;
                return FMOD_OK;
            case PARAM_L2_LOCUT:
                state->params.l2LoCut = value;
                return FMOD_OK;
            case PARAM_L2_HICUT:
                state->params.l2HiCut = value;
                return FMOD_OK;

            case PARAM_RUMBLE_LEVEL:
                state->params.rumbleLevel = value;
                return FMOD_OK;
            case PARAM_RUMBLE_LOCUT:
                state->params.rumbleLoCut = value;
                return FMOD_OK;
            case PARAM_RUMBLE_HICUT:
                state->params.rumbleHiCut = value;
                return FMOD_OK;

            case PARAM_SHOWER_LEVEL:
                state->params.showerLevel = value;
                return FMOD_OK;
            case PARAM_SHOWER_LOCUT:
                state->params.showerLoCut = value;
                return FMOD_OK;
            case PARAM_SHOWER_HICUT:
                state->params.showerHiCut = value;
                return FMOD_OK;
                
            default:
                return FMOD_ERR_INVALID_PARAM;
        }
    }

    FMOD_RESULT F_CALL ThunderStorm_SetParameterInt(FMOD_DSP_STATE *dsp_state, int index, int value) {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL ThunderStorm_SetParameterBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL value) {
        auto *state = static_cast<Framework::ThunderStormState*>(dsp_state->plugindata);
        if (!state) return FMOD_ERR_INVALID_PARAM;
        
        switch (index) {
            case PARAM_THUNDER_TRIGGER:
                state->params.thunderTrigger = (value != 0);
                return FMOD_OK;
            default:
                return FMOD_ERR_INVALID_PARAM;
        }
    }

    FMOD_RESULT F_CALL ThunderStorm_GetParameterFloat(FMOD_DSP_STATE *dsp_state, int index, float *value, char *valuestr) {
        auto *state = static_cast<Framework::ThunderStormState*>(dsp_state->plugindata);
        if (!state) return FMOD_ERR_INVALID_PARAM;
        
        switch (index) {
            case PARAM_RAIN_INTENSITY:
                if (value) *value = state->params.rainIntensity;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.rainIntensity * 100));
                return FMOD_OK;
            case PARAM_THUNDER_DISTANCE:
                if (value) *value = state->params.thunderDistance;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dm", static_cast<int>(state->params.thunderDistance));
                return FMOD_OK;
            case PARAM_THUNDER_FREQ:
                if (value) *value = state->params.thunderFreq;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.thunderFreq * 100));
                return FMOD_OK;
            case PARAM_INDOOR_OUTDOOR:
                if (value) *value = state->params.indoorOutdoor;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.indoorOutdoor * 100));
                return FMOD_OK;
            case PARAM_RAIN_VOLUME:
                if (value) *value = state->params.rainVolume;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.2fx", state->params.rainVolume);
                return FMOD_OK;
            case PARAM_THUNDER_VOLUME:
                if (value) *value = state->params.thunderVolume;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.2fx", state->params.thunderVolume);
                return FMOD_OK;
            case PARAM_THUNDER_STRIKE:
                if (value) *value = state->params.thunderStrike;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.thunderStrike * 100));
                return FMOD_OK;
            case PARAM_THUNDER_RUMBLE:
                if (value) *value = state->params.thunderRumble;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.thunderRumble * 100));
                return FMOD_OK;
            case PARAM_THUNDER_GROWL:
                if (value) *value = state->params.thunderGrowl;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.thunderGrowl * 100));
                return FMOD_OK;
                
            case PARAM_L1_LEVEL:
                if (value) *value = state->params.l1Level;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.2f", state->params.l1Level);
                return FMOD_OK;
            case PARAM_L1_DENSITY:
                if (value) *value = state->params.l1Density;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.l1Density * 100));
                return FMOD_OK;
            case PARAM_L1_COLOR:
                if (value) *value = state->params.l1Color;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(1200.0f + 4500.0f * state->params.l1Color));
                return FMOD_OK;
            case PARAM_L1_VISCOSITY:
                if (value) *value = state->params.l1Viscosity;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.1fs", 2.0f + 10.0f * state->params.l1Viscosity);
                return FMOD_OK;
            case PARAM_L1_MIN:
                if (value) *value = state->params.l1Min;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.3f", state->params.l1Min);
                return FMOD_OK;
            case PARAM_L1_MAX:
                if (value) *value = state->params.l1Max;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.3f", state->params.l1Max);
                return FMOD_OK;
            case PARAM_L1_LOCUT:
                if (value) *value = state->params.l1LoCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(50.0f + state->params.l1LoCut * 2000.0f));
                return FMOD_OK;
            case PARAM_L1_HICUT:
                if (value) *value = state->params.l1HiCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(2000.0f + (1.0f - state->params.l1HiCut) * 18000.0f));
                return FMOD_OK;
                
            case PARAM_L2_LEVEL:
                if (value) *value = state->params.l2Level;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.2f", state->params.l2Level);
                return FMOD_OK;
            case PARAM_L2_DENSITY:
                if (value) *value = state->params.l2Density;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%d", static_cast<int>(state->params.l2Density * 100));
                return FMOD_OK;
            case PARAM_L2_COLOR:
                if (value) *value = state->params.l2Color;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(100.0f + 850.0f * state->params.l2Color));
                return FMOD_OK;
            case PARAM_L2_VISCOSITY:
                if (value) *value = state->params.l2Viscosity;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.1fs", 6.0f + 18.0f * state->params.l2Viscosity);
                return FMOD_OK;
            case PARAM_L2_MIN:
                if (value) *value = state->params.l2Min;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.3f", state->params.l2Min);
                return FMOD_OK;
            case PARAM_L2_MAX:
                if (value) *value = state->params.l2Max;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.3f", state->params.l2Max);
                return FMOD_OK;
            case PARAM_L2_LOCUT:
                if (value) *value = state->params.l2LoCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(50.0f + state->params.l2LoCut * 2000.0f));
                return FMOD_OK;
            case PARAM_L2_HICUT:
                if (value) *value = state->params.l2HiCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(2000.0f + (1.0f - state->params.l2HiCut) * 18000.0f));
                return FMOD_OK;
 
            case PARAM_RUMBLE_LEVEL:
                if (value) *value = state->params.rumbleLevel;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.2f", state->params.rumbleLevel);
                return FMOD_OK;
            case PARAM_RUMBLE_LOCUT:
                if (value) *value = state->params.rumbleLoCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(20.0f + state->params.rumbleLoCut * 580.0f));
                return FMOD_OK;
            case PARAM_RUMBLE_HICUT:
                if (value) *value = state->params.rumbleHiCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(300.0f + state->params.rumbleHiCut * 5700.0f));
                return FMOD_OK;
 
            case PARAM_SHOWER_LEVEL:
                if (value) *value = state->params.showerLevel;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%.2f", state->params.showerLevel);
                return FMOD_OK;
            case PARAM_SHOWER_LOCUT:
                if (value) *value = state->params.showerLoCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(100.0f + state->params.showerLoCut * 4400.0f));
                return FMOD_OK;
            case PARAM_SHOWER_HICUT:
                if (value) *value = state->params.showerHiCut;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%dHz", static_cast<int>(2000.0f + state->params.showerHiCut * 16000.0f));
                return FMOD_OK;
                
            default:
                return FMOD_ERR_INVALID_PARAM;
        }
    }

    FMOD_RESULT F_CALL ThunderStorm_GetParameterInt(FMOD_DSP_STATE *dsp_state, int index, int *value, char *valuestr) {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_RESULT F_CALL ThunderStorm_GetParameterBool(FMOD_DSP_STATE *dsp_state, int index, FMOD_BOOL *value, char *valuestr) {
        auto *state = static_cast<Framework::ThunderStormState*>(dsp_state->plugindata);
        if (!state) return FMOD_ERR_INVALID_PARAM;
        
        switch (index) {
            case PARAM_THUNDER_TRIGGER:
                if (value) *value = state->params.thunderTrigger ? 1 : 0;
                if (valuestr) snprintf(valuestr, FMOD_DSP_GETPARAM_VALUESTR_LENGTH, "%s", state->params.thunderTrigger ? "True" : "False");
                return FMOD_OK;
            default:
                return FMOD_ERR_INVALID_PARAM;
        }
    }
}