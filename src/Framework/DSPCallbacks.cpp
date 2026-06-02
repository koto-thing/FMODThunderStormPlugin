#include "DSPCallbacks.h"
#include "ThunderStormInternalData.h"
#include "../InterfaceAdapter/ParameterDesc.h"

#include <algorithm>
#include <cstring>
#include <new>

#include "FMODAllocator.h"

namespace ThunderStorm::Framework {
    FMOD_RESULT F_CALL ThunderStorm_Create(FMOD_DSP_STATE *dspState) {
        if (!dspState) {
            return FMOD_ERR_INVALID_PARAM;
        }
        
        InterfaceAdapter::InitParameterDescs();
        
        if (!dspState->functions) {
            return FMOD_ERR_INTERNAL;
        }
        
        FMOD_MEMORY_ALLOC_CALLBACK alloc_callback = dspState->functions->alloc;
        if (!alloc_callback) {
            return FMOD_ERR_INTERNAL;
        }
        
        auto *state = (ThunderStormState*)alloc_callback(sizeof(ThunderStormState), FMOD_MEMORY_NORMAL, __FILE__);
        if (state == nullptr) {
            return FMOD_ERR_MEMORY;
        }
        
        new (state) ThunderStormState();
        
        int sampleRate = 48000;
        if (dspState->functions->getsamplerate) {
            dspState->functions->getsamplerate(dspState, &sampleRate);
        }
        
        void* engineMem = alloc_callback(sizeof(UseCase::ThunderStormEngine), FMOD_MEMORY_NORMAL, __FILE__);
        if (!engineMem) {
            state->~ThunderStormState();
            dspState->functions->free(state, FMOD_MEMORY_NORMAL, __FILE__);
            return FMOD_ERR_MEMORY;
        }
        
        state->engine = new (engineMem) UseCase::ThunderStormEngine(static_cast<float>(sampleRate), 3.0f, dspState);
        
        dspState->plugindata = state;
        
        return FMOD_OK;
    }
    
    FMOD_RESULT F_CALL ThunderStorm_Release(FMOD_DSP_STATE *dspState) {
        if (!dspState) {
            return FMOD_ERR_INVALID_PARAM;
        }
        
        if (!dspState->functions) {
            return FMOD_ERR_INTERNAL;
        }
        
        FMOD_MEMORY_FREE_CALLBACK freeCallback = dspState->functions->free;
        if (!freeCallback) {
            return FMOD_ERR_INTERNAL;
        }
        
        auto *state = static_cast<ThunderStormState*>(dspState->plugindata);
        if (state) {
            if (state->engine) {
                state->engine->~ThunderStormEngine();
                freeCallback(state->engine, FMOD_MEMORY_NORMAL, __FILE__);
            }
            
            state->~ThunderStormState();
            freeCallback(state, FMOD_MEMORY_NORMAL, __FILE__);
        }
        
        dspState->plugindata = nullptr;
        
        return FMOD_OK;
    }
    
    FMOD_RESULT F_CALL ThunderStorm_Reset(FMOD_DSP_STATE *dspState) {
        if (!dspState || !dspState->plugindata) {
            return FMOD_ERR_INVALID_PARAM;
        }
        
        auto *state = static_cast<ThunderStormState*>(dspState->plugindata);
        if (state->engine) {
            state->engine->Reset();
        }
        
        return FMOD_OK;
    }
    
    FMOD_RESULT F_CALL ThunderStorm_Process(FMOD_DSP_STATE *dspState, unsigned int length,
        const FMOD_DSP_BUFFER_ARRAY *inBuffers, FMOD_DSP_BUFFER_ARRAY *outBuffers,
        FMOD_BOOL inputsIdle, FMOD_DSP_PROCESS_OPERATION op) {
        
        if (!dspState || !outBuffers) {
            return FMOD_ERR_INVALID_PARAM;
        }
        
        auto *state = static_cast<ThunderStormState*>(dspState->plugindata);
        if (!state || !state->engine) {
            return FMOD_ERR_INVALID_PARAM;
        }
        
        if (op == FMOD_DSP_PROCESS_QUERY) {
            outBuffers->speakermode = FMOD_SPEAKERMODE_STEREO;
            if (outBuffers->buffernumchannels) {
                outBuffers->buffernumchannels[0] = 2;
            }
            if (outBuffers->bufferchannelmask) {
                outBuffers->bufferchannelmask[0] = FMOD_CHANNELMASK_FRONT_LEFT | FMOD_CHANNELMASK_FRONT_RIGHT;
            }
            return FMOD_OK;
        }
        
        if (op != FMOD_DSP_PROCESS_PERFORM) {
            return FMOD_OK;
        }
        
        if (outBuffers->numbuffers == 0 || outBuffers->buffers == nullptr) {
            return FMOD_OK;
        }
        
        // Output buffer
        float *out = outBuffers->buffers[0];
        int outChs = outBuffers->buffernumchannels[0];
        
        // Allocate temporary buffers for stereo generation using the custom allocator
        FMODAllocator<float> allocator(dspState);
        std::vector<float, FMODAllocator<float>> tempL(length, 0.0f, allocator);
        std::vector<float, FMODAllocator<float>> tempR(length, 0.0f, allocator);
        
        // Run procedural thunderstorm engine
        state->engine->Process(tempL.data(), tempR.data(), length, state->params);
        
        // Reset the trigger so it behaves as a one-shot trigger
        state->params.thunderTrigger = false;
        
        // Write procedural generator audio directly to the output
        for (unsigned int s = 0; s < length; ++s) {
            float outL = tempL[s];
            float outR = tempR[s];
            
            if (outChs >= 2) {
                out[s * outChs + 0] = outL;
                out[s * outChs + 1] = outR;
                for (int c = 2; c < outChs; ++c) {
                    out[s * outChs + c] = 0.0f;
                }
            } else if (outChs == 1) {
                out[s] = (outL + outR) * 0.5f;
            }
        }
        
        return FMOD_OK;
    }
}
