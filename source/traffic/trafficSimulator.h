#ifndef TRAFFIC_SIMULATOR_H
#define TRAFFIC_SIMULATOR_H

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include "statistics.h"
#include "trafficDirector.h" //todo rename
#include "threadSchedulingContext.h"
#include "firstComeFirstServed.h"
#include "timeBasedPreemption.h"
#include "../definitions.h"

namespace traffic {
    class TrafficSimulator {
        public:
            Statistics* stats = nullptr;
            ThreadSchedulingContext* context;
            std::vector<std::thread> cars;

            TrafficSimulator(Strategy strategy, int numCars, bool hasPedestrians); // todo delete pedestrians
            ~TrafficSimulator();
            Statistics* runSimulation();
            void setThreadSchedulingStrategy(Strategy strategy);

        private:
            TrafficDirector* trafficDirector;
            Strategy strategy;
            int numCars;
            bool hasPedestrians;

            void intiThreadSchedulingStrategy(Strategy strategy);
            void startCars(int numCars);
            void waitForSimToEnd();
    };
    
    TrafficSimulator::TrafficSimulator(Strategy strategy, int numCars, bool hasPedestrians)
    : strategy(strategy), hasPedestrians(hasPedestrians), numCars(numCars) {
        intiThreadSchedulingStrategy(strategy);
        trafficDirector = (strategy == Strategy::TIME_BASED_PREEMPTION) ? new TrafficDirector(context, 250) : nullptr;
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
        startCars(numCars); // todo loop sim for more stats?
        waitForSimToEnd();
        if (strategy == Strategy::TIME_BASED_PREEMPTION) {
            trafficDirector->stop();
        }
        stats->printData();
        return stats;
    }

    void TrafficSimulator::setThreadSchedulingStrategy(Strategy strategy) {
        if(stats) {
            delete stats;
        }
        stats = new Statistics();

        switch(strategy) {
            case Strategy::FIRST_COME_FIRST_SERVED:
                context->set_strategy(std::make_unique<FirstComeFirstServed>(stats));
                break;
            case Strategy::TIME_BASED_PREEMPTION:
                context->set_strategy(std::make_unique<TimeBasedPreemption>(stats));
                break;
            default:
                context->set_strategy(std::make_unique<FirstComeFirstServed>(stats));
                break;
        }
    }

    void TrafficSimulator::intiThreadSchedulingStrategy(Strategy strategy) {
        if(stats) {
            delete stats;
        }
        stats = new Statistics();

        switch(strategy) {
            case Strategy::FIRST_COME_FIRST_SERVED:
                context = new ThreadSchedulingContext(std::make_unique<FirstComeFirstServed>(stats));
                break;
            case Strategy::TIME_BASED_PREEMPTION:
                context = new ThreadSchedulingContext(std::make_unique<TimeBasedPreemption>(stats));
                break;
            default:
                context = new ThreadSchedulingContext(std::make_unique<TimeBasedPreemption>(stats));
                break;
        }
    }

    void TrafficSimulator::startCars(int numCars) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dirDist(0, 1);

        for (int i = 0; i < numCars; ++i) {
            Direction direction = (dirDist(gen) == 0) ? Direction::EAST : Direction::WEST;
            cars.emplace_back(&ThreadSchedulingContext::enterStreet, context, direction);
        }
    }

    void TrafficSimulator::waitForSimToEnd() {
        for(auto& thread : cars) {
            thread.join();
        }
    }
}

#endif