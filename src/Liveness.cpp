#include "Liveness.h"

LivenessInfo computeUseDef(Function& fn) {
    LivenessInfo li;
    return li;
}

LivenessResult LivenessAnalysis::analyse(Function& fn) {
    LivenessInfo livenessInfo = computeUseDef(fn);
    LivenessResult lr;
    return lr;
}