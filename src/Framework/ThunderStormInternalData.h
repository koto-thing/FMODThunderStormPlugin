#ifndef THUNDERSTORMPLUGIN_THUNDERSTORMINTERNALDATA_H
#define THUNDERSTORMPLUGIN_THUNDERSTORMINTERNALDATA_H

#include "../UseCase/ThunderStormEngine.h"
#include "../UseCase/ThunderStormParameters.h"

namespace ThunderStorm::Framework {
    struct ThunderStormState {
        UseCase::ThunderStormParameters params;
        UseCase::ThunderStormEngine* engine;
        
        ThunderStormState() : engine(nullptr) { }
    };
}

#endif // THUNDERSTORMPLUGIN_THUNDERSTORMINTERNALDATA_H