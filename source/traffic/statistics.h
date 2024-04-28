#ifndef STATISTICS_H
#define STATISTICS_H

#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <unordered_map>

#include "../definitions.h"

namespace traffic {
    const int MAX_OCCUPANCY = 3;

    enum Time {START, ENTER, LEAVE};

    using ClockType = std::chrono::high_resolution_clock;

    class Statistics {
        public:
            Statistics();
            ~Statistics();
            void recordStats(std::thread::id threadId, Time timeToUpdate, Direction direction, std::chrono::time_point<ClockType> time);
            void recordOccupancy(int numCarsOnStreet, Direction direction);
            void printData();
            const std::vector<std::thread::id>& getCrossingOrder() const;
            const std::unordered_map<std::thread::id, CrossingDatum>& getCrossingData() const;
            // const int (&getOccupancy())[int(Direction::NUM_DIRECTIONS)][MAX_OCCUPANCY + 1]; todo for stats screen

        private:
            std::chrono::high_resolution_clock::time_point startTime;
            std::vector<std::thread::id> crossingOrder;
            std::unordered_map<std::thread::id, CrossingDatum> crossingData;
            int occupancy[int(Direction::NUM_DIRECTIONS)][MAX_OCCUPANCY + 1];
            std::mutex statsMutex;

    };
    
    Statistics::Statistics() : startTime(std::chrono::high_resolution_clock::now()), occupancy{{0}} {
    }

    Statistics::~Statistics() {}

    void Statistics::recordStats(std::thread::id threadId, Time timeToUpdate, Direction direction, std::chrono::time_point<ClockType> time) {
        std::lock_guard<std::mutex> statsLock(statsMutex);

        if (crossingData.find(threadId) == crossingData.end()) {
            crossingData.emplace(threadId, CrossingDatum{direction, {}, {}, {}});
            crossingOrder.push_back(threadId);
        }
        
        auto timeSinceStart = std::chrono::duration<double>(time - startTime);

        if (timeToUpdate == START) {
            // std::cout << "startWaitTime for" << threadId << "is " << timeToUpdate << std::endl;
            crossingData[threadId].startWaitTime = timeSinceStart * MULTIPLY_TIME_BY_FACTOR;
        } else if (timeToUpdate == ENTER) {
            // std::cout << "enterTime for" << threadId << "is " << timeToUpdate << std::endl;
            crossingData[threadId].enterTime = timeSinceStart * MULTIPLY_TIME_BY_FACTOR;
        } else if (timeToUpdate == LEAVE) {
            // std::cout << "leaveTime for" << threadId << "is " << timeToUpdate << std::endl;
            crossingData[threadId].leaveTime = timeSinceStart * MULTIPLY_TIME_BY_FACTOR;
        } 
    }

    void Statistics::recordOccupancy(int numCarsOnStreet, Direction direction) {
        std::lock_guard<std::mutex> statsLock(statsMutex);
        occupancy[direction][numCarsOnStreet]++;
    }

    void Statistics::printData() {
        std::cout << "Done here's the stats:" << std::endl;

        for(const auto& threadId : crossingOrder) {
            CrossingDatum data = crossingData[threadId];

            std::cout   << "Key: " << threadId 
                        << ", Value: { direction: " << data.direction 
                        << ", start: " << data.startWaitTime.count()
                        << ", enter: " << data.enterTime.count()
                        << ", leave: " << data.leaveTime.count()
                        << " }" << std::endl;
        }

        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            for (int j = 1; j < MAX_OCCUPANCY + 1; j++) {
                std::cout << occupancy[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    const std::vector<std::thread::id>& Statistics::getCrossingOrder() const {
        return crossingOrder;
    }

    const std::unordered_map<std::thread::id, CrossingDatum>& Statistics::getCrossingData() const {
        return crossingData;
    }

    // const int (&getOccupancy())[int(Direction::NUM_DIRECTIONS)][MAX_OCCUPANCY + 1] { todo for stats screen
    //     return occupancy;
    // }
}

#endif