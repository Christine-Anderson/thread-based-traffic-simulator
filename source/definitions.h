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