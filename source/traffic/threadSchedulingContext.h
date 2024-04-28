#ifndef THREAD_SCHEDULING_CONTEXT_H
#define THREAD_SCHEDULING_CONTEXT_H

#include <memory>
#include <iostream>

#include "threadSchedulingStrategy.h"
#include "../definitions.h"

namespace traffic {
    class ThreadSchedulingContext {
        public:
            explicit ThreadSchedulingContext(std::unique_ptr<ThreadSchedulingStrategy> strategy = nullptr);
            void set_strategy(std::unique_ptr<ThreadSchedulingStrategy> strategy);
            void enterStreet(Direction direction);
            void changeStreetDirection();
        
        private:
            std::unique_ptr<ThreadSchedulingStrategy> strategy_;
    };

    ThreadSchedulingContext::ThreadSchedulingContext(std::unique_ptr<ThreadSchedulingStrategy> strategy) 
        : strategy_(std::move(strategy)) {}

    void ThreadSchedulingContext::set_strategy(std::unique_ptr<ThreadSchedulingStrategy> strategy){
        strategy_ = std::move(strategy);
    }
    
    void ThreadSchedulingContext::enterStreet(Direction direction) {
        if (strategy_) {
            strategy_->enterStreet(direction);
        }
    }

    void ThreadSchedulingContext::changeStreetDirection() {
        if (strategy_) {
            strategy_->changeStreetDirection();
        }
    }
}

#endif