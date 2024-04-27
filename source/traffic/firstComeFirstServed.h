#ifndef FIRST_COME_FIRST_SERVED_H
#define FIRST_COME_FIRST_SERVED_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>

#include "statistics.h"
#include "../definitions.h"

namespace traffic {
    class FirstComeFirstServed : public TrafficManagementStrategy {
        public:
            FirstComeFirstServed(Statistics* stats);
            ~FirstComeFirstServed();
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
    
    FirstComeFirstServed::FirstComeFirstServed(Statistics* stats) : stats(stats), numCarsOnStreet(0) {}

    FirstComeFirstServed::~FirstComeFirstServed() {}

    void FirstComeFirstServed::enterStreet(Direction carDirection) {
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

    void FirstComeFirstServed::driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
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

    void FirstComeFirstServed::waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        // std::cout << threadId << " waiting to go " << directionToString(carDirection) << std::endl;
    }

    void FirstComeFirstServed::leaveStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), LEAVE, carDirection, std::chrono::high_resolution_clock::now());
        numCarsOnStreet--;
    }

    void FirstComeFirstServed::assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        assert(numCarsOnStreet <= MAX_OCCUPANCY);
        assert(carDirection == streetDirection);
    }

    void FirstComeFirstServed::changeStreetDirection() {
        // do nothing
    }
}

#endif