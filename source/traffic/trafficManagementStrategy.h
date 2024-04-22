#ifndef TRAFFIC_MANAGEMENT_STRATEGY_H
#define TRAFFIC_MANAGEMENT_STRATEGY_H

#include "../definitions.h"

namespace traffic {
    class TrafficManagementStrategy {
        public:
            virtual ~TrafficManagementStrategy() = default;
            virtual void enterStreet(Direction carDirection) = 0;
            virtual void changeStreetDirection() = 0;
    };
}

#endif