#ifndef TRAFFIC_DIRECTOR_H
#define TRAFFIC_DIRECTOR_H

#include <iostream>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include "threadSchedulingContext.h"

namespace traffic {
    class TrafficDirector {
        public:
            TrafficDirector(ThreadSchedulingContext* context, int intervalMicrosec);
            void start();
            void stop();
        
        private:
            ThreadSchedulingContext* context;
            int intervalMicrosec;
            bool running;
            std::thread directorThread;

            void run();
    };
    
    TrafficDirector::TrafficDirector(ThreadSchedulingContext* context, int intervalMicrosec) 
        : context(context), intervalMicrosec(intervalMicrosec), running(false) {}

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

            while (running) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime).count();
                int remainingTime = intervalMicrosec - static_cast<int>(elapsedTime);

                if(remainingTime <= 0) {
                    context->changeStreetDirection();
                }
            }
        }
    }
}

#endif