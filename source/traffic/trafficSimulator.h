#ifndef TRAFFIC_SIMULATOR_H
#define TRAFFIC_SIMULATOR_H

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include "statistics.h"
#include "trafficDirector.h" //todo rename
#include "trafficManagementContext.h"
#include "firstInFirstOut.h"
#include "../definitions.h"

namespace traffic {
    class TrafficSimulator { // todo Thread scheduling simulator
        public:
            Statistics* stats;
            TrafficManagementContext* context;
            std::vector<std::thread> cars;

            TrafficSimulator(int numCarsEast, int numCarsWest, bool hasPedestrians, bool hasTrafficDirector); // todo need to add in the vars we want user to be able to change
            ~TrafficSimulator();
            Statistics* runSimulation();
            void setTrafficManagementStrategy(Strategy strategy);

        private:
            TrafficDirector* trafficDirector;
            int numCarsEast;
            int numCarsWest;
            bool hasPedestrians;

            void createCars(int numCars, Direction direction);
            void waitForSimToEnd();
    };
    
    TrafficSimulator::TrafficSimulator(int numCarsEast, int numCarsWest, bool hasPedestrians, bool hasTrafficDirector) 
        : numCarsEast(numCarsEast), numCarsWest(numCarsWest) {
        stats = new Statistics();
        context = new TrafficManagementContext(std::make_unique<FirstInFirstOut>(stats));

        this->hasPedestrians = hasPedestrians; // todo haven't done yet
        if (hasTrafficDirector) {
            this->trafficDirector = new TrafficDirector();
        }
    }

    TrafficSimulator::~TrafficSimulator() {
        // todo clean up
    }

    Statistics* TrafficSimulator::runSimulation() {
        createCars(numCarsEast, Direction::EAST); // todo loop sim for more stats?
        createCars(numCarsWest, Direction::WEST);
        waitForSimToEnd();
        stats->printData();
        return stats;
    }

    void TrafficSimulator::setTrafficManagementStrategy(Strategy strategy) {
        switch(strategy) {
            case Strategy::FIRST_IN_FIRST_OUT:
                context->set_strategy(std::make_unique<FirstInFirstOut>());
                break;
            default:
                context->set_strategy(std::make_unique<FirstInFirstOut>());
                break;
        }
    }

    void TrafficSimulator::createCars(int numCars, Direction direction) {
        for(int i = 0; i < numCars; i++) {
            cars.emplace_back(&TrafficManagementContext::enterStreet, context, direction);
        }
    }

    void TrafficSimulator::waitForSimToEnd() { // todo add pedestrians (on or off, like an interrupt to the threads, make all threads wait)
        for(auto& thread : cars) { // todo no scheduling mech vs traffic director/dif mechs
            thread.join();
        }
    }
}

#endif
