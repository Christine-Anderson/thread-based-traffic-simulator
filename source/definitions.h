#ifndef DEFINITIONS_H
#define DEFINITIONS_H

const int MULTIPLY_TIME_BY_FACTOR = 50000;

enum Direction {
    EAST,
    WEST,
    NUM_DIRECTIONS
};

enum CarState {
    HIDDEN,
    WAITING_HIDDEN,
    WAITING_VISIBLE,
    CROSSING,
    DONE
};

enum Strategy {
    FIRST_IN_FIRST_OUT,
    TIME_BASED_PREEMPTION
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

struct crossingDatum {
    Direction direction;
    std::chrono::duration<double> startWaitTime;
    std::chrono::duration<double> enterTime;
    std::chrono::duration<double> leaveTime;
};

#endif