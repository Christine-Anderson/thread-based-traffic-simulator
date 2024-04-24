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
        : stats(stats), numCarsOnStreet(0), canEnterEast(false), canEnterWest(false), changeDirection(false) {
    }

    TimeBasedPreemption::~TimeBasedPreemption() {}

    void TimeBasedPreemption::enterStreet(Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), START, carDirection, std::chrono::high_resolution_clock::now());

        std::cout << std::this_thread::get_id() << " waiting to go " << directionToString(carDirection) << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << std::this_thread::get_id() << " enterStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;

        if(numCarsOnStreet == 0) {
            if (changeDirection) {
                streetDirection = (streetDirection == Direction::EAST)? Direction::WEST : Direction::EAST;
                changeDirection = false;
            }
            if (carDirection == streetDirection) {
                std::cout << "empty street drive on through" << std::endl;
                driveThroughStreet(streetLock, carDirection);
            } else {
                std::cout << "street going the wrong direction, wait your turn" << std::endl;
                waitToEnterStreet(streetLock, carDirection);
            }
        } else if (numCarsOnStreet > 0 && numCarsOnStreet < MAX_OCCUPANCY) { 
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

    void TimeBasedPreemption::driveThroughStreet(Direction carDirection) {
        std::cout << std::this_thread::get_id() << " driveThroughStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;

        stats->recordStats(std::this_thread::get_id(), ENTER, carDirection, std::chrono::high_resolution_clock::now());
        numCarsOnStreet++
        assertStreetOccupancyConstraints(streetLock, carDirection);
        stats->recordOccupancy(numCarsOnStreet, carDirection);

        streetLock.unlock();

        int j = 0;
        for (int i = 0; i < 100000; i++) {
            j++;
        }
        
        leaveStreet(carDirection);
    }

    void TimeBasedPreemption::waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        std::cout << std::this_thread::get_id() << " waiting to go " << directionToString(carDirection) << std::endl;
        // old 
        switch (carDirection) {
            case Direction::EAST:
                enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
                driveThroughStreet(streetLock, carDirection);
                break;
            case Direction::WEST:
                enterWestBound.wait(streetLock, [this]{ return canEnterWest; });
                driveThroughStreet(streetLock, carDirection);
                break;
            default:
                enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
                driveThroughStreet(streetLock, carDirection);
                break;
        }

        // gpt
        // bool canEnter = false;
        // switch (carDirection) {
        //     case Direction::EAST:
        //         enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
        //         canEnter = canEnterEast;
        //         break;
        //     case Direction::WEST:
        //         enterWestBound.wait(streetLock, [this]{ return canEnterWest; });
        //         canEnter = canEnterWest;
        //         break;
        //     default:
        //         enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
        //         canEnter = canEnterEast;
        //         break;
        // }

        // if(canEnter) {
        //     driveThroughStreet(streetLock, carDirection);
        // }
    }

    void TimeBasedPreemption::leaveStreet(Direction carDirection) {
        std::unique_lock<std::mutex> streetLock(streetMutex);

        stats->recordStats(std::this_thread::get_id(), LEAVE, carDirection, std::chrono::high_resolution_clock::now());
        numCarsOnStreet--;
        std::cout << std::this_thread::get_id() << " leaveStreet: " << numCarsOnStreet << " cars on street going " << directionToString(streetDirection) << std::endl;
        
        if (changeDirection && numCarsOnStreet == 0) {
            streetDirection = (streetDirection == Direction::EAST)? Direction::WEST : Direction::EAST;
            changeDirection = false;
        }
        
        if (numCarsOnStreet < MAX_OCCUPANCY) {
            signalCarToEnter(streetLock);
        }
    }

    void TimeBasedPreemption::signalCarToEnter(std::unique_lock<std::mutex>& streetLock) {
        // old
        // switch (streetDirection) {
        //     case Direction::EAST:
        //         {
        //             std::lock_guard<std::mutex> streetLock(streetMutex);
        //             canEnterEast = true;
        //         }
        //         enterEastBound.notify_all();
        //         break;
        //     case Direction::WEST:
        //         {
        //             std::lock_guard<std::mutex> streetLock(streetMutex);
        //             canEnterWest = true;
        //         }
        //         enterWestBound.notify_all();
        //         break;
        //     default:
        //         {
        //             std::lock_guard<std::mutex> streetLock(streetMutex);
        //             canEnterEast = true;
        //         }
        //         enterEastBound.notify_all();
        //         break;
        // }
//gpt
        // bool canEnter = false;
        
        // switch (streetDirection) {
        //     case Direction::EAST:
        //         canEnter = canEnterEast;
        //         enterEastBound.notify_all();
        //         break;
        //     case Direction::WEST:
        //         canEnter = canEnterWest;
        //         enterWestBound.notify_all();
        //         break;
        //     default:
        //         canEnter = canEnterEast;
        //         enterEastBound.notify_all();
        //         break;
        
        // if(!canEnter) {
        //     if (streetDirection == Direction::EAST) {
        //         canEnterEast = true;
        //     } else {
        //         canEnterWest = true;
        //     }
        // }
        
        switch (streetDirection) {
            case Direction::EAST:
                canEnterEast = true;
                enterEastBound.notify_all();
                break;
            case Direction::WEST:
                canEnterWest = true;
                enterWestBound.notify_all();
                break;
            default:
                canEnterEast = true;
                enterEastBound.notify_all();
                break;
        }
    }

    void TimeBasedPreemption::changeStreetDirection() {
        std::cout << std::this_thread::get_id() << " waiting to change direction" << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << std::this_thread::get_id() << " changed direction to " << directionToString(streetDirection) << std::endl;

        changeDirection = true;
        if (numCarsOnStreet == 0) {
            changeDirection = false;
            streetDirection = (streetDirection == Direction::EAST)? Direction::WEST : Direction::EAST;
            signalCarToEnter(streetLock);
        }
    }

    void TimeBasedPreemption::assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        
        if (numCarsOnStreet > MAX_OCCUPANCY) {
            std::cout << std::this_thread::get_id() << "num cars on street too high: " << numCarsOnStreet << std::endl;
        }
        if (carDirection != streetDirection) {
            std::cout << std::this_thread::get_id() << "car driving wrong way " << std::endl;
        }
        // assert(numCarsOnStreet <= MAX_OCCUPANCY);
        // assert(carDirection == streetDirection);
    }
}

#endif