#ifndef TRAFFIC_MANAGEMENT_CONTEXT_H
#define TRAFFIC_MANAGEMENT_CONTEXT_H

#include <memory>
#include <iostream>

#include "trafficManagementStrategy.h"
#include "../definitions.h"

namespace traffic {
    class TrafficManagementContext {
        public:
            explicit TrafficManagementContext(std::unique_ptr<TrafficManagementStrategy> strategy = nullptr);
            void set_strategy(std::unique_ptr<TrafficManagementStrategy> strategy);
            void enterStreet(Direction direction);
            void changeStreetDirection();
        
        private:
            std::unique_ptr<TrafficManagementStrategy> strategy_;
    };

    TrafficManagementContext::TrafficManagementContext(std::unique_ptr<TrafficManagementStrategy> strategy) 
        : strategy_(std::move(strategy)) {}

    void TrafficManagementContext::set_strategy(std::unique_ptr<TrafficManagementStrategy> strategy){
        strategy_ = std::move(strategy);
    }
    
    void TrafficManagementContext::enterStreet(Direction direction) {
        if (strategy_) {
            strategy_->enterStreet(direction);
        }
    }

    void TrafficManagementContext::changeStreetDirection() {
        if (strategy_) {
            strategy_->changeStreetDirection();
        }
    }
}

#endif