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
            std::cout << "Context: Strategy set\n"; //todo delete
            strategy_->enterStreet(direction);
        } else {
            std::cout << "Context: Strategy isn't set\n";
        }
    }
}

#endif