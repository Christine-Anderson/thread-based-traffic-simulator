#ifndef FIRST_IN_FIRST_OUT_H
#define FIRST_IN_FIRST_OUT_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>

#include "statistics.h"
#include "../definitions.h"

namespace traffic {
    class FirstInFirstOut : public TrafficManagementStrategy {
        public:
            FirstInFirstOut(Statistics* stats);
            ~FirstInFirstOut();
            void enterStreet(Direction carDirection) override;
            void changeStreetDirection() override;
        
        private:
            Statistics* stats;
            Direction streetDirection;
            int numCarsOnStreet;
            std::mutex streetMutex;

            void driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void leaveStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
    };
    
    FirstInFirstOut::FirstInFirstOut(Statistics* stats) : stats(stats), numCarsOnStreet(0) {}

    FirstInFirstOut::~FirstInFirstOut() {}

    void FirstInFirstOut::enterStreet(Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), START, carDirection, std::chrono::high_resolution_clock::now());

        // std::cout << threadId << " waiting to go " << directionToString(carDirection) << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        // std::cout << threadId << " enterStreet: " << numCarsOnStreet << " cars on street going " << streetDirection << std::endl;

        if (numCarsOnStreet == 0) {
            // std::cout << "empty street drive on through" << threadId << std::endl;
            streetDirection = carDirection;
            driveThroughStreet(streetLock, carDirection);
        }
    }

    void FirstInFirstOut::driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), ENTER, carDirection, std::chrono::high_resolution_clock::now());

        numCarsOnStreet++;
        // std::cout << threadId << " driveThroughStreet: " << numCarsOnStreet << " cars on street going " << streetDirection << std::endl;
        int j = 0;
        for (int i = 0; i < 100000; i++) {
            j++;
        }
        assertStreetOccupancyConstraints(streetLock, carDirection);
        stats->recordOccupancy(numCarsOnStreet, carDirection);
        leaveStreet(streetLock, carDirection);
    }

    void FirstInFirstOut::waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        // std::cout << threadId << " waiting to go " << directionToString(carDirection) << std::endl;
    }

    void FirstInFirstOut::leaveStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), LEAVE, carDirection, std::chrono::high_resolution_clock::now());
        numCarsOnStreet--;
    }

    void FirstInFirstOut::assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        assert(numCarsOnStreet <= MAX_OCCUPANCY);
        assert(carDirection == streetDirection);
    }

    void FirstInFirstOut::changeStreetDirection() {
        // do nothing
    }
}

#endif