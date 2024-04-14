#ifndef STREET_H
#define STREET_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "statistics.h"
#include "../definitions.h"

namespace traffic {
    class Street {
        public:
            Street(Statistics* stats);
            ~Street();
            void enterStreet(Direction carDirection);
        
        private:
            Statistics* stats;
            Direction streetDirection;
            int numCarsOnStreet;
            // bool canEnterEast;
            // bool canEnterWest;
            std::mutex streetMutex;
            // std::condition_variable enterEastBound;
            // std::condition_variable enterWestBound;

            void driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void leaveStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void signalCarToEnter(std::unique_lock<std::mutex>& streetLock);
            void assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
    };
    
    Street::Street(Statistics* stats) : stats(stats), numCarsOnStreet(0) {
        // this->canEnterEast = false;
        // this->canEnterWest = false;
    }

    Street::~Street() {
        // todo clean up
    }

    void Street::enterStreet(Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), START, carDirection, std::chrono::high_resolution_clock::now());

        // std::cout << threadId << " waiting to go " << directionToString(carDirection) << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        // std::cout << threadId << " enterStreet: " << numCarsOnStreet << " cars on street going " << streetDirection << std::endl;

        if (numCarsOnStreet == 0) {
            // std::cout << "empty street drive on through" << threadId << std::endl;
            streetDirection = carDirection;
            driveThroughStreet(streetLock, carDirection);
        }
        // if (numCarsOnStreet == 0) {
        //     std::cout << "empty street drive on through" << std::endl;
        //     streetDirection = carDirection;
        //     driveThroughStreet(streetLock, carDirection);
        // } else {
        //     std::cout << "street occupied, wait your turn" << std::endl;
        //     waitToEnterStreet(streetLock, carDirection);
        // }
    }

    void Street::driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
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

    void Street::waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        // std::cout << threadId << " waiting to go " << directionToString(carDirection) << std::endl;

        // if (carDirection == EAST) {
        //     enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
        //     driveThroughStreet(streetLock, carDirection);
        // } else if (carDirection == WEST) {
        //     enterWestBound.wait(streetLock, [this]{ return canEnterWest; });
        //     driveThroughStreet(streetLock, carDirection);
        // }
    }

    void Street::leaveStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        stats->recordStats(std::this_thread::get_id(), LEAVE, carDirection, std::chrono::high_resolution_clock::now());

        numCarsOnStreet--;
        // std::cout << threadId << " leaveStreet: " << numCarsOnStreet << " cars on street going " << streetDirection << std::endl;
        // if (numCarsOnStreet < MAX_OCCUPANCY) {
        //     signalCarToEnter(streetLock);
        // } else {
        //     streetLock.unlock();
        // }
    }

    void Street::signalCarToEnter(std::unique_lock<std::mutex>& streetLock) {      
        // if (streetDirection == EAST) {
        //     {
        //         std::lock_guard<std::mutex> streetLock(streetMutex);
        //         canEnterEast = true;
        //     }
        //     enterEastBound.notify_all();
        // } else if (streetDirection == WEST) {
        //     {
        //         std::lock_guard<std::mutex> streetLock(streetMutex);
        //         canEnterWest = true;
        //     }
        //     enterWestBound.notify_all();
        // }
    }

    void Street::assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        assert(numCarsOnStreet <= MAX_OCCUPANCY);
        assert(carDirection == streetDirection);
    }
} // todo return both first iteration for simulation and stats for graph

#endif