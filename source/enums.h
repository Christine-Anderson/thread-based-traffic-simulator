#ifndef ENUMS_H
#define ENUMS_H

enum Direction {
    EAST,
    WEST,
    NUM_DIRECTIONS
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

#endif