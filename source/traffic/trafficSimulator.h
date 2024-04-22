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
#include "timeBasedPreemption.h"
#include "../definitions.h"

namespace traffic {
    class TrafficSimulator { // todo Thread scheduling simulator
        public:
            Statistics* stats = nullptr;
            TrafficManagementContext* context;
            std::vector<std::thread> cars;

            TrafficSimulator(Strategy strategy, int numCarsEast, int numCarsWest, bool hasPedestrians); // todo need to add in the vars we want user to be able to change
            ~TrafficSimulator();
            Statistics* runSimulation();
            void setTrafficManagementStrategy(Strategy strategy);

        private:
            TrafficDirector* trafficDirector;
            Strategy strategy;
            int numCarsEast;
            int numCarsWest;
            bool hasPedestrians;

            void initTrafficManagementStrategy(Strategy strategy);
            void startCars(int numCars, Direction direction);
            void waitForSimToEnd();
    };
    
    TrafficSimulator::TrafficSimulator(Strategy strategy, int numCarsEast, int numCarsWest, bool hasPedestrians) // todo random number of cars of east and west
        : strategy(strategy), hasPedestrians(hasPedestrians), numCarsEast(numCarsEast), numCarsWest(numCarsWest) {
        initTrafficManagementStrategy(strategy);
        trafficDirector = (strategy == Strategy::TIME_BASED_PREEMPTION) ? new TrafficDirector(context, 1000) : nullptr;
    }

    TrafficSimulator::~TrafficSimulator() {
        delete stats;
        delete context;
        if (trafficDirector) {
            delete trafficDirector;
        }
    }

    Statistics* TrafficSimulator::runSimulation() {
        if (strategy == Strategy::TIME_BASED_PREEMPTION) {
            trafficDirector->start();
        }
        startCars(numCarsEast, Direction::EAST); // todo loop sim for more stats?
        startCars(numCarsWest, Direction::WEST);
        waitForSimToEnd();
        if (strategy == Strategy::TIME_BASED_PREEMPTION) {
            trafficDirector->stop();
        }
        stats->printData();
        return stats;
    }

    void TrafficSimulator::setTrafficManagementStrategy(Strategy strategy) {
        if(stats) {
            delete stats;
        }
        stats = new Statistics();

        switch(strategy) {
            case Strategy::FIRST_IN_FIRST_OUT:
                context->set_strategy(std::make_unique<FirstInFirstOut>(stats));
                break;
            case Strategy::TIME_BASED_PREEMPTION:
                context->set_strategy(std::make_unique<TimeBasedPreemption>(stats));
                break;
            default:
                context->set_strategy(std::make_unique<FirstInFirstOut>(stats));
                break;
        }
    }

    void TrafficSimulator::initTrafficManagementStrategy(Strategy strategy) {
        if(stats) {
            delete stats;
        }
        stats = new Statistics();

        switch(strategy) {
            case Strategy::FIRST_IN_FIRST_OUT:
                context = new TrafficManagementContext(std::make_unique<FirstInFirstOut>(stats));
                break;
            case Strategy::TIME_BASED_PREEMPTION:
                context = new TrafficManagementContext(std::make_unique<TimeBasedPreemption>(stats));
                break;
            default:
                context = new TrafficManagementContext(std::make_unique<TimeBasedPreemption>(stats));
                break;
        }
    }

    void TrafficSimulator::startCars(int numCars, Direction direction) {
        for(int i = 0; i < numCars; i++) {
            cars.emplace_back(&TrafficManagementContext::enterStreet, context, direction);
        }
    }

    void TrafficSimulator::waitForSimToEnd() { // todo add pedestrians (like an interrupt to the threads, make all threads wait)
        for(auto& thread : cars) {
            thread.join();
        }
    }
}

#endif