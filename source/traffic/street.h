#ifndef STREET_H
#define STREET_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace traffic {
    enum Direction {EAST, WEST};
    int MAX_OCCUPANCY = 3;

    class Street {
        public:
            Street();
            ~Street();
            void enterStreet(Direction carDirection);
        
        private:
            Direction streetDirection;
            int numCarsOnStreet;
            bool canEnterStreet;
            // bool canEnterEast;
            // bool canEnterWest;
            std::mutex streetMutex;
            std::condition_variable signalEnterStreet;
            // std::condition_variable enterEastBound;
            // std::condition_variable enterWestBound;

            void driveThroughStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection);
            void leaveStreet(std::unique_lock<std::mutex>& streetLock);
            void signalCarToEnter(std::unique_lock<std::mutex>& streetLock);
            void assertStreetOccupancyConstraints(std::unique_lock<std::mutex>& streetLock, Direction carDirection);

    };
    
    Street::Street() {
        std::cout << "street" << std::endl;
        this->numCarsOnStreet = 0;
        // this->canEnterEast = false;
        // this->canEnterWest = false;
        this->canEnterStreet = false;
    }

    Street::~Street() {
        // todo clean up
    }

    void Street::enterStreet(Direction carDirection) {
        std::cout << "we made it" << std::endl;
        std::unique_lock<std::mutex> streetLock(streetMutex);
        std::cout << "enterStreet: " << numCarsOnStreet << " cars on street going " << streetDirection << std::endl;
        if (numCarsOnStreet == 0) {
            std::cout << "empty street drive on through" << std::endl;
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
        numCarsOnStreet++;
        std::cout << "driveThroughStreet: " << numCarsOnStreet << " cars on street going " << streetDirection << std::endl;
        int j = 0;
        for (int i = 0; i < 100000; i++) {
            j++;
        }
        assertStreetOccupancyConstraints(streetLock, carDirection);
        leaveStreet(streetLock);
    }

    void Street::waitToEnterStreet(std::unique_lock<std::mutex>& streetLock, Direction carDirection) {
        std::cout << "waiting to go " << carDirection << std::endl;

        signalEnterStreet.wait(streetLock, [this]{ return canEnterStreet; });
        canEnterStreet = false;
        driveThroughStreet(streetLock, carDirection);

        // if (carDirection == EAST) {
        //     enterEastBound.wait(streetLock, [this]{ return canEnterEast; });
        //     driveThroughStreet(streetLock, carDirection);
        // } else if (carDirection == WEST) {
        //     enterWestBound.wait(streetLock, [this]{ return canEnterWest; });
        //     driveThroughStreet(streetLock, carDirection);
        // }
    }

    void Street::leaveStreet(std::unique_lock<std::mutex>& streetLock) {
        numCarsOnStreet--;
        std::cout << "leaveStreet: " << numCarsOnStreet << " cars on street going " << streetDirection << std::endl;
        // if (numCarsOnStreet < MAX_OCCUPANCY) {
        //     signalCarToEnter(streetLock);
        // } else {
        //     streetLock.unlock();
        // }
    }

    void Street::signalCarToEnter(std::unique_lock<std::mutex>& streetLock) {
        {
            std::lock_guard<std::mutex> streetLock(streetMutex);
            canEnterStreet = true;
            std::cout << "signal car to enter" << std::endl;
        }
        streetLock.unlock();
        signalEnterStreet.notify_all();
        
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