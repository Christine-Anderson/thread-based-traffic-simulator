#ifndef TIME_BASED_PREEMPTION_H
#define TIME_BASED_PREEMPTION_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>

#include "statistics.h"
#include "../definitions.h"

namespace traffic {
    class TimeBasedPreemption : public TrafficManagementStrategy {
        public:
            TimeBasedPreemption(Statistics* stats);
            ~TimeBasedPreemption();
            void enterStreet(Direction carDirection) override;
            void changeStreetDirection() override;
        
        private:
            Statistics* stats;
            Direction streetDirection;
            int numCarsOnStreet;
            bool canEnterEast;
            bool canEnterWest;
            bool changeDirection;
            std::mutex streetMutex;
            std::condition_variable enterEastBound;
            std::condition_variable enterWestBound;

            void driveThroughStreet(Direction carDirection);
            void waitToEnterStreet(Direction carDirection);
            void leaveStreet(Direction carDirection);
            void signalCarToEnter();
            void assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
    };
    
    TimeBasedPreemption::TimeBasedPreemption(Statistics* stats) 
        : stats(stats), numCarsOnStreet(0), canEnterEast(false), canEnterWest(false), changeDirection(false) {
    }

    TimeBasedPreemption::~TimeBasedPreemption() {}

    void TimeBasedPreemption::enterStreet(Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), START, carDirection, std::chrono::high_resolution_clock::now());

        std::cout << std::this_thread::get_id() << " waiting to go " << directionToString(carDirection) << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << std::this_thread::get_id() << " enterStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;

        if (numCarsOnStreet < MAX_OCCUPANCY && carDirection == streetDirection) { // christine todo
            std::cout << "Driving through the street..." << std::endl;
            driveThroughStreet(carDirection);
        } else {
            std::cout << "Waiting to enter the street..." << std::endl;
            waitToEnterStreet(carDirection);
        }
    }

    void TimeBasedPreemption::driveThroughStreet(Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), ENTER, carDirection, std::chrono::high_resolution_clock::now());

        {
            std::lock_guard<std::mutex> lock(streetMutex);
            numCarsOnStreet++;
            std::cout << std::this_thread::get_id() << " driveThroughStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;
            if (numCarsOnStreet > MAX_OCCUPANCY) {
                std::cout << std::this_thread::get_id() << "num cars on street too high: " << numCarsOnStreet << std::endl;
            }
            if (carDirection != streetDirection) {
                std::cout << std::this_thread::get_id() << "car driving wrong way " << std::endl;
            }
            stats->recordOccupancy(numCarsOnStreet, carDirection);
        }
        
        int j = 0;
        for (int i = 0; i < 100000; i++) {
            j++;
        }

        // std::this_thread::sleep_for(std::chrono::milliseconds(100)); todo try this 
        
        leaveStreet(carDirection);
    }

    void TimeBasedPreemption::waitToEnterStreet(Direction carDirection) {
        std::cout << std::this_thread::get_id() << " waiting to go " << directionToString(carDirection) << std::endl;
        
        std::unique_lock<std::mutex> streetLock(streetMutex);

        if (carDirection == Direction::EAST) {
            enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
        } else {
            enterWestBound.wait(streetLock, [this]{ return canEnterWest; });
        }
    }

    void TimeBasedPreemption::leaveStreet(Direction carDirection) {
        std::unique_lock<std::mutex> streetLock(streetMutex);

        stats->recordStats(std::this_thread::get_id(), LEAVE, carDirection, std::chrono::high_resolution_clock::now());
        {
            std::lock_guard<std::mutex> lock(streetMutex);
            numCarsOnStreet--;
        }
        std::cout << std::this_thread::get_id() << " leaveStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;
        
        if (changeDirection && numCarsOnStreet == 0) {
            streetDirection = (streetDirection == Direction::EAST) ? Direction::WEST : Direction::EAST;
            changeDirection = false;
        }
    
        if (numCarsOnStreet < MAX_OCCUPANCY) {
            signalCarToEnter();
        }
    }

    void TimeBasedPreemption::signalCarToEnter() {
        if (streetDirection == Direction::EAST) {
            canEnterEast = true;
            enterEastBound.notify_one();
        } else {
            canEnterWest = true;
            enterWestBound.notify_one();
        }
    }

    void TimeBasedPreemption::changeStreetDirection() {
        std::cout << std::this_thread::get_id() << " waiting to change direction" << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << std::this_thread::get_id() << " changed direction to " << directionToString(streetDirection) << std::endl;

        changeDirection = true;

        // Wait until the street is empty before changing direction
        while (numCarsOnStreet > 0) {
            streetLock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            streetLock.lock();
        }

        streetDirection = (streetDirection == Direction::EAST) ? Direction::WEST : Direction::EAST;
        changeDirection = false;

        signalCarToEnter(); 
    }

    void TimeBasedPreemption::assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        assert(numCarsOnStreet <= MAX_OCCUPANCY);
        assert(carDirection == streetDirection);
    }
}

#endif