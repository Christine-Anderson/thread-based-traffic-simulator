#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <iostream>
#include <chrono>
#include <ctime>

const int MULTIPLY_TIME_BY_FACTOR = 50000;

enum Direction {
    EAST,
    WEST,
    NUM_DIRECTIONS
};

enum CarState {
    HIDDEN,
    WAITING1,
    WAITING2,
    CROSSING,
    DONE
};

std::string directionToString(Direction direction) {
    switch(direction) {
        case Direction::EAST:
            return "East";
        case Direction::WEST:
            return "West";
        case Direction::NUM_DIRECTIONS:
            return "2";
    }
}

struct crossingDatum {
    Direction direction;
    std::chrono::duration<double> startWaitTime;
    std::chrono::duration<double> enterTime;
    std::chrono::duration<double> leaveTime;
};

#endif