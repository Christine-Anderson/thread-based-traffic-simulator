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

            void driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void leaveStreet(Direction carDirection);
            void signalCarToEnter(std::unique_lock<std::mutex>& streetLock);
            void assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
    };
    
    TimeBasedPreemption::TimeBasedPreemption(Statistics* stats) 
        : stats(stats), numCarsOnStreet(0), streetDirection(Direction::EAST), canEnterEast(false), canEnterWest(false), changeDirection(false) {
    }

    TimeBasedPreemption::~TimeBasedPreemption() {}

    void TimeBasedPreemption::enterStreet(Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), START, carDirection, std::chrono::high_resolution_clock::now());

        std::cout << std::this_thread::get_id() << "enterStreet getting mutex for " << directionToString(carDirection) << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << std::this_thread::get_id() << " enterStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;

        // if (numCarsOnStreet == 0 and changeDirection) {
        //     streetDirection = (streetDirection == Direction::EAST)? Direction::WEST : Direction::EAST;
        //     changeDirection = false;
        // }
        
        if (numCarsOnStreet < MAX_OCCUPANCY) { 
            if(carDirection == streetDirection) {   
                std::cout << "there is some space on the street, drive on through" << std::endl;
                driveThroughStreet(streetLock, carDirection);
            } else {
                std::cout << "street in wrong direction, wait your turn" << std::endl;
                waitToEnterStreet(streetLock, carDirection);
            }
        } else { 
            std::cout << "street at max occupancy, wait your turn" << std::endl;
            waitToEnterStreet(streetLock, carDirection);
        }
    }

    void TimeBasedPreemption::driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), ENTER, carDirection, std::chrono::high_resolution_clock::now());
        numCarsOnStreet++;
        std::cout << std::this_thread::get_id() << " driveThroughStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;
        stats->recordOccupancy(numCarsOnStreet, carDirection);

        if (numCarsOnStreet > MAX_OCCUPANCY) {
            std::cout << std::this_thread::get_id() << "num cars on street too high: " << numCarsOnStreet << std::endl;
        }
        if (carDirection != streetDirection) {
            std::cout << std::this_thread::get_id() << "car driving wrong way " << std::endl;
        }

        streetLock.unlock();

        int j = 0;
        for (int i = 0; i < 100000; i++) {
            j++;
        }
        
        leaveStreet(carDirection);
    }

    void TimeBasedPreemption::waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        std::cout << std::this_thread::get_id() << " waitToEnterStreet in direction: " << directionToString(carDirection) << std::endl;
        if (carDirection ==  Direction::EAST) {
            while(numCarsOnStreet == MAX_OCCUPANCY  || carDirection != streetDirection) {
                enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
            }
            driveThroughStreet(streetLock, carDirection);
        } else {
            while(numCarsOnStreet == MAX_OCCUPANCY  || carDirection != streetDirection) {
                enterWestBound.wait(streetLock, [this]{ return canEnterWest; });
            }
            driveThroughStreet(streetLock, carDirection);
        }
    }

    void TimeBasedPreemption::leaveStreet(Direction carDirection) {
        std::cout << std::this_thread::get_id() << " leaveStreet getting mutex to go " << directionToString(carDirection) << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << std::this_thread::get_id() << " leaveStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;

        stats->recordStats(std::this_thread::get_id(), LEAVE, carDirection, std::chrono::high_resolution_clock::now());
        numCarsOnStreet--;
        
        if (changeDirection && numCarsOnStreet == 0) {
            changeDirection = false;
            streetDirection = (streetDirection == Direction::EAST)? Direction::WEST : Direction::EAST;
        }
        if (numCarsOnStreet < MAX_OCCUPANCY) {
            signalCarToEnter(streetLock);
        }
    }

    void TimeBasedPreemption::signalCarToEnter(std::unique_lock<std::mutex>& streetLock) {
        if (streetDirection == Direction::EAST) {
            canEnterEast = true;
            enterEastBound.notify_all();
        } else {
            canEnterWest = true;
            enterWestBound.notify_all();
        }
    }

    void TimeBasedPreemption::changeStreetDirection() {
        std::cout << std::this_thread::get_id() << "changeStreetDirection getting the mutex" << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << std::this_thread::get_id() << "changeStreetDirection changed direction to " << directionToString(streetDirection) << std::endl;

        changeDirection = true;
        if (numCarsOnStreet == 0) {
            changeDirection = false;
            streetDirection = (streetDirection == Direction::EAST)? Direction::WEST : Direction::EAST;
            signalCarToEnter(streetLock);
        }

        // changeDirection = true;

        // // Wait until the street is empty before changing direction
        // while (numCarsOnStreet > 0) {
        //     streetLock.unlock();
        //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //     streetLock.lock();
        // }

        // streetDirection = (streetDirection == Direction::EAST) ? Direction::WEST : Direction::EAST;
        // changeDirection = false;

        // signalCarToEnter(streetLock); 
    }

    void TimeBasedPreemption::assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {        
        assert(numCarsOnStreet <= MAX_OCCUPANCY);
        assert(carDirection == streetDirection);
    }
}

#endif