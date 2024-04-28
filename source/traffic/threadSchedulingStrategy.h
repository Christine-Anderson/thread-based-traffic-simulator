#ifndef THREAD_SCHEDULING_STRATEGY_H
#define THREAD_SCHEDULING_STRATEGY_H

#include "../definitions.h"

namespace traffic {
    class ThreadSchedulingStrategy {
        public:
            virtual ~ThreadSchedulingStrategy() = default;
            virtual void enterStreet(Direction carDirection) = 0;
            virtual void changeStreetDirection() = 0;
    };
}

#endif