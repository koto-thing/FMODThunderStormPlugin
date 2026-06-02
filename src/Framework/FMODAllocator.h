#ifndef FMODTHUNDERSTORMPLUGIN_FMODALLOCATOR_H
#define FMODTHUNDERSTORMPLUGIN_FMODALLOCATOR_H

#include "fmod.h"
#include "fmod_dsp.h"
#include <cstddef>
#include <new>

namespace ThunderStorm::Framework {
    
    template <class T>
    struct FMODAllocator {
        typedef T value_type;
        FMOD_DSP_STATE* dspState;
        
        FMODAllocator(FMOD_DSP_STATE *state) noexcept : dspState(state) { }
        
        template <class U> constexpr FMODAllocator(const FMODAllocator<U>& other) noexcept : dspState(other.dspState) { }
        
        [[nodiscard]] T* allocate(std::size_t n) {
            if (n > std::size_t(-1) / sizeof(T)) throw std::bad_alloc();
            if (dspState && dspState->functions && dspState->functions->alloc) {
                if (auto p = static_cast<T*>(dspState->functions->alloc(static_cast<unsigned int>(n * sizeof(T)), FMOD_MEMORY_NORMAL, __FILE__))) {
                    return p;
                }
            }
            
            throw std::bad_alloc();
        }
        
        void deallocate(T* p, std::size_t n) noexcept {
            if (p && dspState && dspState->functions && dspState->functions->free) {
                dspState->functions->free(p, FMOD_MEMORY_NORMAL, __FILE__);
            }
        }
    };
    
    template <class T, class U>
    bool operator==(const FMODAllocator<T>& a, const FMODAllocator<U>& b) {
        return a.dspState == b.dspState;
    }
    
    template <class T, class U>
    bool operator!=(const FMODAllocator<T>& a, const FMODAllocator<U>& b) {
        return !(a == b);
    }
}

#endif // FMODTHUNDERSTORMPLUGIN_FMODALLOCATOR_H