#ifndef TRAFFIC_SIMULATOR_H
#define TRAFFIC_SIMULATOR_H

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include "street.h"
#include "statistics.h"
#include "trafficDirector.h" //todo rename

namespace traffic {
    class TrafficSimulator { // todo Thread scheduling simulator
        public:
            Statistics* stats;
            Street* street;
            std::vector<std::thread> cars;

            TrafficSimulator(int numCarsEast, int numCarsWest, bool hasPedestrians, bool hasTrafficDirector); // todo need to add in the vars we want user to be able to change
            ~TrafficSimulator();
            void runSimulation();

        private:
            TrafficDirector* trafficDirector;
            int numCarsEast;
            int numCarsWest;
            bool hasPedestrians;

            void createCars(int numCars, Direction direction);
            void waitForSimToEnd();
    };
    
    TrafficSimulator::TrafficSimulator(int numCarsEast, int numCarsWest, bool hasPedestrians, bool hasTrafficDirector) :
    stats(new Statistics()), street(new Street(stats)), numCarsEast(numCarsEast), numCarsWest(numCarsWest) {
        this->hasPedestrians = hasPedestrians; // todo haven't done yet
        if (hasTrafficDirector) {
            this->trafficDirector = new TrafficDirector();
        }
    }

    TrafficSimulator::~TrafficSimulator() {
        // todo clean up
    }

    void TrafficSimulator::runSimulation() {
        createCars(numCarsEast, EAST); // todo loop sim for more stats?
        createCars(numCarsWest, WEST);
        waitForSimToEnd();
        std::cout << "done here's the stats:" << std::endl;
        stats->printData();
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
