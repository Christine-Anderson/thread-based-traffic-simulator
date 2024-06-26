#ifndef DEFINITIONS_H
#define DEFINITIONS_H

const int MULTIPLY_TIME_BY_FACTOR = 50000;

enum Direction {
    EAST,
    WEST,
    NUM_DIRECTIONS
};

enum SimulationState {
    MENU,
    SIMULATION_RUNNING,
    SIMULATION_DONE
};

enum CarState {
    HIDDEN,
    WAITING_HIDDEN,
    WAITING_VISIBLE,
    CROSSING,
    DONE
};

enum Strategy {
    FIRST_COME_FIRST_SERVED,
    TIME_BASED_PREEMPTION
};

struct CrossingDatum {
    Direction direction;
    std::chrono::duration<double> startWaitTime;
    std::chrono::duration<double> enterTime;
    std::chrono::duration<double> leaveTime;
};

struct MenuData {
    bool runSimulation;
    Strategy threadSchedulingStrategy;
    int numThreads;
};

std::string directionToString(Direction direction) {
    switch(direction) {
        case Direction::EAST:
            return "EAST";
        case Direction::WEST:
            return "WEST";
        case Direction::NUM_DIRECTIONS:
            return "2";
    }
}

std::string stateToString(CarState direction) {
    switch(direction) {
        case CarState::HIDDEN:
            return "HIDDEN";
        case CarState::WAITING_HIDDEN:
            return "WAITING_HIDDEN";
        case CarState::WAITING_VISIBLE:
            return "WAITING_VISIBLE";
        case CarState::CROSSING:
            return "CROSSING";
        case CarState::DONE:
            return "DONE";
    }
}

#endif