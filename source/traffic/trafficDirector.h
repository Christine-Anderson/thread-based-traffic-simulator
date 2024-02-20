#ifndef TRAFFIC_DIRECTOR_H
#define TRAFFIC_DIRECTOR_H

#include <iostream>
#include <thread>
#include <mutex>

namespace traffic {
    class TrafficDirector { // todo 
        public:
            TrafficDirector();
            ~TrafficDirector();
        
        private:
            int variables;

    };
    
    TrafficDirector::TrafficDirector() {
        std::cout << "create traffic director" << std::endl;
    }

    TrafficDirector::~TrafficDirector() {
    }
}

#endif