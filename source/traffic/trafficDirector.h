#ifndef TRAFFIC_DIRECTOR_H
#define TRAFFIC_DIRECTOR_H

#include <iostream>
#include <thread>
#include <mutex>

namespace traffic {
    class TrafficDirector {
        public:
            TrafficDirector();
            ~TrafficDirector();
        
        private:
            int variables;

    };
    
    TrafficDirector::TrafficDirector() {

    }

    TrafficDirector::~TrafficDirector() {
    }
}

#endif