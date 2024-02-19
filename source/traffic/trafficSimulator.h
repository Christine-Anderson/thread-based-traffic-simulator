#ifndef TRAFFIC_SIMULATOR_H
#define TRAFFIC_SIMULATOR_H

#include <iostream>
#include <thread>
#include <mutex>

#include "street.h"
#include "trafficDirector.h"

namespace traffic { //todo separate .h and .cpp files
    class TrafficSimulator { //todo make more complicated?
        public:
            TrafficSimulator(int numCarsEast, int numCarsWest); // todo need to add in the vars we want user to be able to change
            ~TrafficSimulator();
            std::string runSimulation();
            void createCars(int numCars, Direction direction);
            void startCars();

        private:
            Street* street;
            int numCarsEast;
            int numCarsWest;
            std::vector<std::thread> cars;
    };
    
    TrafficSimulator::TrafficSimulator(int numCarsEast, int numCarsWest) {
        this->numCarsEast = numCarsEast;
        this->numCarsWest = numCarsWest;
        this->street = new Street();
    }

    TrafficSimulator::~TrafficSimulator() {
        // todo clean up
    }

    std::string TrafficSimulator::runSimulation() {
        //todo add histogram tracking of some sort; 

        createCars(numCarsEast, EAST);
        createCars(numCarsWest, WEST);

        TrafficDirector trafficDirector(); //todo

        startCars();
    }

    void TrafficSimulator::createCars(int numCars, Direction direction) {
        for(int i = 0; i < numCars; i++) {
            cars.push_back(std::thread (std::bind(Street::enterStreet, &street, direction)));
        }
    }

    void TrafficSimulator::startCars() {
        for(auto& thread : cars) {
            thread.join();
        }
    }
}

#endif
