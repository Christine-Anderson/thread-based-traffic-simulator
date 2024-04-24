#ifndef CAR_H
#define CAR_H

#include <vector>
#include <random>
#include <string>
#include <cmath>

#include "../lib/olcPixelGameEngine.h"
#include "../definitions.h"

class Car {
    const olc::vf2d OFFSET = {256, 256};
    const olc::vf2d START_EAST = {-256, 490};
    const olc::vf2d START_WEST = {1256, 360};
    const std::vector<olc::vf2d> WAIT_TO_GO_EAST = {{125, 490}};
    const std::vector<olc::vf2d> WAIT_TO_GO_WEST = {{900, 360}};
    const std::vector<olc::vf2d> PATH_CROSSING_EAST = {{1256, 490}};
    const std::vector<olc::vf2d> PATH_CROSSING_WEST = {{700, 490}, {350, 490}, {200, 360}, {-256, 360}};
    const float DEFAULT_SPEED = 500.0;
    std::string IMAGES_FOLDER = "./source/assets/vehicles/";
    const std::vector<std::string> CAR_TYPES = { "hatchbackSports", "sedan", "truck", "van"};

    public:
        Car(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, std::chrono::duration<double> normalizeStartTimeTo, std::thread::id carId, crossingDatum crossingData);
        ~Car();
        void update(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime);
        Direction getDirection();
        CarState getState();
        void setState(CarState newState);

    private:
        olc::PixelGameEngine* engine;
        std::chrono::high_resolution_clock::time_point startTime;
        std::thread::id carId;
        CarState state;
        crossingDatum crossingData;
        olc::vf2d currPosition;
        std::map<std::string, olc::Decal*> carDecals;
        std::chrono::duration<double> normalizeStartTimeTo;
        size_t currentPathIndex;
        std::vector<olc::vf2d> path;
        float speed;
        int carType;

        std::map<std::string, olc::Decal*> generateCarDecals(Direction direction);
        void updateState(std::chrono::high_resolution_clock::time_point currentTime);
        void initState(CarState state);
        void updatePosition(float elapsedTime);
        void render(olc::vf2d currPosition, std::string cardinalDirection, float elapsedTime);
        std::string moveAlongPath(const std::vector<olc::vf2d>& path, float speed, float elapsedTime);
        std::string vectorToCardinalDirection(const olc::vf2d& directionVector);
        float calculateAverageSpeedAlongPath(const std::vector<olc::vf2d>& path);
};

Car::Car(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, std::chrono::duration<double> normalizeStartTimeTo, std::thread::id carId, crossingDatum crossingData) 
: engine(engine), startTime(startTime), normalizeStartTimeTo(normalizeStartTimeTo), carId(carId), crossingData(crossingData) {
    currPosition = (crossingData.direction == Direction::EAST) ? currPosition = START_EAST : currPosition = START_WEST;
    carDecals = generateCarDecals(crossingData.direction);
    currentPathIndex = 0;
    state = CarState::HIDDEN;
    speed = DEFAULT_SPEED;
}

Car::~Car() {
    for (auto& pair : carDecals) {
        delete pair.second;
    }
}

std::map<std::string, olc::Decal*> Car::generateCarDecals(Direction direction) {
    std::random_device rand;
    std::mt19937 gen(rand());
    std::uniform_int_distribution<int> distrib(0, CAR_TYPES.size());
    carType = distrib(gen);
    
    std::map<std::string, olc::Decal*> carSpritesDecal;
    std::string imagePath = IMAGES_FOLDER + CAR_TYPES[carType];

    carSpritesDecal["E"] = new olc::Decal(new olc::Sprite(imagePath + "_N.png"));
    carSpritesDecal["W"] = new olc::Decal(new olc::Sprite(imagePath + "_S.png"));
    carSpritesDecal["S"] = new olc::Decal(new olc::Sprite(imagePath + "_E.png"));
    carSpritesDecal["N"] = new olc::Decal(new olc::Sprite(imagePath + "_W.png"));
    carSpritesDecal["SE"] = new olc::Decal(new olc::Sprite(imagePath + "_NE.png"));
    carSpritesDecal["NE"] = new olc::Decal(new olc::Sprite(imagePath + "_NW.png"));
    carSpritesDecal["SW"] = new olc::Decal(new olc::Sprite(imagePath + "_SE.png"));
    carSpritesDecal["NW"] = new olc::Decal(new olc::Sprite(imagePath + "_SW.png"));

    return carSpritesDecal;
}

void Car::update(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime) {
    updateState(currentTime);
    updatePosition(elapsedTime);
}

void Car::updateState(std::chrono::high_resolution_clock::time_point currentTime) {
    std::chrono::duration<double> startWaitTime = crossingData.startWaitTime - normalizeStartTimeTo;
    std::chrono::duration<double> enterTime = crossingData.enterTime - normalizeStartTimeTo;
    std::chrono::duration<double> leaveTime = crossingData.leaveTime - normalizeStartTimeTo;

    auto timeSinceStart = std::chrono::duration<double>(currentTime - startTime);

    if (timeSinceStart > startWaitTime && timeSinceStart < enterTime && state == CarState::HIDDEN && state != CarState::WAITING_HIDDEN) {
        std::cout << "waiting hidden" << std::endl;
        state = CarState::WAITING_HIDDEN;
        initState(state);        
    } else if (timeSinceStart >= enterTime && timeSinceStart <= leaveTime && state == CarState::WAITING_VISIBLE && state != CarState::CROSSING) {
        std::cout << "crossing" << std::endl;
        state = CarState::CROSSING;
        initState(state);
    } else if (timeSinceStart > leaveTime && state != CarState::DONE) {
        std::cout << "done" << std::endl;
        state = CarState::DONE;
    }
}

void Car::initState(CarState state) {
    olc::vf2d startPosition;
    std::vector<olc::vf2d> pathFromStart;

    switch(state) {
        case CarState::WAITING_HIDDEN:
            path = (crossingData.direction == Direction::EAST)? WAIT_TO_GO_EAST : WAIT_TO_GO_WEST;
            currentPathIndex = 0;
            break;
        case CarState::CROSSING:
            path = (crossingData.direction == Direction::EAST)? PATH_CROSSING_EAST : PATH_CROSSING_WEST;
            currentPathIndex = 0;
            pathFromStart = (crossingData.direction == Direction::EAST)? WAIT_TO_GO_EAST : WAIT_TO_GO_WEST;
            pathFromStart.insert(pathFromStart.end(), path.begin(), path.end());
            speed = calculateAverageSpeedAlongPath(pathFromStart);
            break;
        default:
            break;
    }
}

void Car::updatePosition(float elapsedTime) {
    std::string cardinalDirection;

    switch(state) {
        case CarState::WAITING_VISIBLE:
        case CarState::CROSSING:
            cardinalDirection = moveAlongPath(path, speed, elapsedTime);
            render(currPosition, cardinalDirection, elapsedTime);
            break;
        default:
            break;
    }
}

void Car::render(olc::vf2d currPosition, std::string cardinalDirection, float elapsedTime) {
    engine->DrawDecal(currPosition - OFFSET, carDecals[cardinalDirection]);
}

std::string Car::moveAlongPath(const std::vector<olc::vf2d>& path, float speed, float elapsedTime) {
    if(currentPathIndex >= path.size()) {
        return (crossingData.direction == Direction::EAST)? "E": "W";
    }

    olc::vf2d distanceToTarget = path[currentPathIndex] - currPosition;
    olc::vf2d distanceToMove = {0.0, 0.0};
    olc::vf2d directionVector = distanceToTarget.norm();
    // std::cout << "distanceToTarget " << std::to_string(distanceToTarget.x) << std::to_string(distanceToTarget.y) <<std::endl;

    if(distanceToTarget.mag2() > 0.0) {
        distanceToMove = directionVector * speed * elapsedTime;
        // std::cout << "distanceToMove " << std::to_string(distanceToMove.x) << std::to_string(distanceToMove.y) << std::endl;
    }

    if(distanceToTarget.mag2() <= distanceToMove.mag2()) {
        distanceToMove = distanceToTarget;
        currentPathIndex++;
    } 

    currPosition += distanceToMove;

    return vectorToCardinalDirection(directionVector);
}

std::string Car::vectorToCardinalDirection(const olc::vf2d& directionVector) {
    float angleRadians = std::atan2(directionVector.y, directionVector.x);
    float angleDegrees = angleRadians * (180.0f / M_PI);

    // Normalize angle to be within [0, 360) degrees
    if (angleDegrees < 0.0f) {
        angleDegrees += 360.0f;
    }

    std::string cardinalDirection;

    if (angleDegrees >= 337.5f || angleDegrees < 22.5f) {
        cardinalDirection = "E";
    } else if (angleDegrees >= 22.5f && angleDegrees < 67.5f) {
        cardinalDirection = "SE";
    } else if (angleDegrees >= 67.5f && angleDegrees < 112.5f) {
        cardinalDirection = "N";
    } else if (angleDegrees >= 112.5f && angleDegrees < 157.5f) {
        cardinalDirection = "SW";
    } else if (angleDegrees >= 157.5f && angleDegrees < 202.5f) {
        cardinalDirection = "W";
    } else if (angleDegrees >= 202.5f && angleDegrees < 247.5f) {
        cardinalDirection = "NW";
    } else if (angleDegrees >= 247.5f && angleDegrees < 292.5f) {
        cardinalDirection = "S";
    } else if (angleDegrees >= 292.5f && angleDegrees < 337.5f) {
        cardinalDirection = "NE";
    }

    return cardinalDirection;
}

float Car::calculateAverageSpeedAlongPath(const std::vector<olc::vf2d>& path) {
    float totalDistance = 0.0;
    for (size_t i = 1; i < path.size(); i++) {
        olc::vf2d distanceVector = path[i] - path[i - 1];
        totalDistance += distanceVector.mag();
    }
    auto timeDifference = (crossingData.leaveTime) - (crossingData.enterTime);
    double elapsedTimeSeconds = std::chrono::duration<double>(timeDifference).count();
    float averageSpeed = (elapsedTimeSeconds > 0.0) ?  totalDistance / static_cast<float>(elapsedTimeSeconds) : 0.0;
    return averageSpeed;
}

Direction Car::getDirection() {
    return crossingData.direction;
}

CarState Car::getState() {
    return state;
}

void Car::setState(CarState newState) {
    state = newState;
    std::cout << "new state " << stateToString(state) << std::endl;
}

#endif