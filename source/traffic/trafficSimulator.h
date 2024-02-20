#ifndef TRAFFIC_SIMULATOR_H
#define TRAFFIC_SIMULATOR_H

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include "street.h"
#include "trafficDirector.h"

namespace traffic { //todo separate .h and .cpp files
    class TrafficSimulator { // todo Thread scheduling simulator
        public:
            Street* street;
            std::vector<std::thread> cars;

            TrafficSimulator(int numCarsEast, int numCarsWest, bool hasPedestrians, bool hasTrafficDirector); // todo need to add in the vars we want user to be able to change
            ~TrafficSimulator();
            std::string runSimulation();

        private:
            TrafficDirector* trafficDirector;
            int numCarsEast;
            int numCarsWest;
            bool hasPedestrians;

            void createCars(int numCars, Direction direction);
            void waitForSimToEnd();
    };
    
    TrafficSimulator::TrafficSimulator(int numCarsEast, int numCarsWest, bool hasPedestrians, bool hasTrafficDirector) {
        this->street = new Street();
        this->numCarsEast = numCarsEast;
        this->numCarsWest = numCarsWest;
        this->hasPedestrians = hasPedestrians;
        if (hasTrafficDirector) {
            this->trafficDirector = new TrafficDirector();
        }
    }

    TrafficSimulator::~TrafficSimulator() {
        // todo clean up
    }

    std::string TrafficSimulator::runSimulation() {
        //todo add histogram tracking of some sort; 
        std::cout << "start simulation" << std::endl; // todo make function for optional print statements?
        createCars(numCarsEast, EAST);
        createCars(numCarsWest, WEST);
        waitForSimToEnd();
        return "done";
    }

    void TrafficSimulator::createCars(int numCars, Direction direction) {
        for(int i = 0; i < numCars; i++) {
            cars.emplace_back(&Street::enterStreet, street, direction);
        }
    }

    void TrafficSimulator::waitForSimToEnd() { // todo add pedestrians (on or off, like an interrupt to the threads, make all threads wait)
        for(auto& thread : cars) { // todo no scheduling mech vs traffic director/dif mechs
            thread.join();
        }
    }
}

#endif
