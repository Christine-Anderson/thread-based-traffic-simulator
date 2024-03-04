#ifndef STATISTICS_H
#define STATISTICS_H

#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <unordered_map>

#include "../enums.h"

namespace traffic {
    const int MAX_OCCUPANCY = 3;

    enum Time {START, ENTER, LEAVE};

    using ClockType = std::chrono::high_resolution_clock;

    struct crossingDatum {
        Direction direction;
        std::chrono::time_point<ClockType> startWaitTime;
        std::chrono::time_point<ClockType> enterTime;
        std::chrono::time_point<ClockType> leaveTime;
    };

    class Statistics { // todo rename me?
        public:
            Statistics();
            ~Statistics();
            void recordDirection(std::thread::id threadId, Direction direction);
            void recordTime(std::thread::id threadId, Time timeToUpdate, std::chrono::time_point<ClockType> time);
            void recordOccupancy(int numCarsOnStreet, Direction direction);
            void printData();

        private:
            std::unordered_map<std::thread::id, crossingDatum> crossingData;
            int occupancy[int(Direction::NUM_DIRECTIONS)][MAX_OCCUPANCY + 1];
            std::mutex statsMutex;

    };
    
    Statistics::Statistics() : occupancy{{0}} {
    }

    Statistics::~Statistics() {
    }

    void Statistics::recordDirection(std::thread::id threadId, Direction direction) {
        std::unique_lock<std::mutex> statsLock(statsMutex);
        std::chrono::high_resolution_clock::time_point placeHolder = std::chrono::high_resolution_clock::now();
        crossingData.emplace(threadId, crossingDatum{direction, placeHolder, placeHolder, placeHolder});
    }

    void Statistics::recordTime(std::thread::id threadId, Time timeToUpdate, std::chrono::time_point<ClockType> time) {
        std::unique_lock<std::mutex> statsLock(statsMutex);
        if (timeToUpdate == START) {
            std::cout << "startWaitTime for" << threadId << "is " << timeToUpdate << std::endl;
            crossingData[threadId].startWaitTime = time;
        } else if (timeToUpdate == ENTER) {
            std::cout << "enterTime for" << threadId << "is " << timeToUpdate << std::endl;
            crossingData[threadId].enterTime = time;
        } else if (timeToUpdate == LEAVE) {
            std::cout << "leaveTime for" << threadId << "is " << timeToUpdate << std::endl;
            crossingData[threadId].leaveTime = time;
        } 
    }

    void Statistics::recordOccupancy(int numCarsOnStreet, Direction direction) {
        std::unique_lock<std::mutex> statsLock(statsMutex);
        occupancy[direction][numCarsOnStreet]++;
    }

    void Statistics::printData() {
        for (const auto& pair : crossingData) {
            std::cout   << "Key: " << pair.first 
                        << ", Value: { direction: " << pair.second.direction 
                        << ", start: " << pair.second.startWaitTime.time_since_epoch().count()
                        << ", enter: " << pair.second.enterTime.time_since_epoch().count()
                        << ", leave: " << pair.second.leaveTime.time_since_epoch().count()
                        << " }" << std::endl;
        }

        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            for (int j = 1; j < MAX_OCCUPANCY + 1; j++) {
                std::cout << occupancy[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
}

#endif