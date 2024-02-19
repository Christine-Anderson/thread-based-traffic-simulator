#ifndef STREET_H
#define STREET_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace traffic {
    enum Direction {EAST, WEST};

    class Street {
        public:
            Street();
            ~Street();
            void enterStreet();
        
        private:
            Direction direction;
            int numCarsOnStreet;
            bool canChangeDirection;
            std::mutex streetMutex;
            std::condition_variable enterEastBound;
            std::condition_variable enterWestBound;

    };
    
    Street::Street() {
        std::cout << "street" << std::endl;
        direction = EAST;
        numCarsOnStreet = 0;
        canChangeDirection = false;
    }

    Street::~Street() {
    }

    void Street::enterStreet() {
        
    }
}

#endif