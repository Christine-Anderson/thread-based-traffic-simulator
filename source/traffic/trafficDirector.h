#ifndef TRAFFIC_DIRECTOR_H
#define TRAFFIC_DIRECTOR_H

#include <iostream>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include "trafficManagementContext.h"

namespace traffic {
    class TrafficDirector {
        public:
            TrafficDirector(TrafficManagementContext* context, int intervalMillisec);
            void start();
            void stop();
        
        private:
            TrafficManagementContext* context;
            int intervalMillisec;
            bool running;
            std::thread directorThread;

            void run();
    };
    
    TrafficDirector::TrafficDirector(TrafficManagementContext* context, int intervalMillisec) 
        : context(context), intervalMillisec(intervalMillisec), running(false) {}

    void TrafficDirector::start() {
        running = true;
        directorThread = std::thread(&TrafficDirector::run, this);
    }

    void TrafficDirector::stop() {
        running = false;
        if(directorThread.joinable()) {
            directorThread.join();
        }
    }

    void TrafficDirector::run() {
        while (running) {
            auto startTime = std::chrono::steady_clock::now();

            while (true) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
                int remainingTime = intervalMillisec - static_cast<int>(elapsedTime);

                if(remainingTime <= 0) {
                    context->changeStreetDirection();
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
}

#endif