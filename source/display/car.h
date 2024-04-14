#ifndef CAR_H
#define CAR_H

#include <vector>

#include "../lib/olcPixelGameEngine.h"
#include "../definitions.h"

class Car {
    const olc::vf2d WAIT_TO_GO_EAST1 = {-256, 94};
    const olc::vf2d WAIT_TO_GO_WEST1 = {744, -30}; // todo all positions
    const olc::vf2d WAIT_TO_GO_EAST2 = {0, 94}; 
    const olc::vf2d WAIT_TO_GO_WEST2 = {250, 250};
    const olc::vf2d END_CROSSING_EAST = {1256, 94};
    const olc::vf2d END_CROSSING_WEST = {-256, -30};
    const olc::vf2d DIRECTION_EAST = {1, 0};
    const olc::vf2d DIRECTION_WEST = {-1, 0}; 
    const float SPEED = 100.0;

    public:
        Car(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, std::thread::id carId, crossingDatum crossingData);
        ~Car();
        void update(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime);

    private:
        olc::PixelGameEngine* engine;
        std::thread::id carId;
        CarState state;
        crossingDatum crossingData;
        olc::vf2d currPosition;
        olc::Sprite *sprite = nullptr;
        olc::Decal *decal = nullptr;
        std::chrono::high_resolution_clock::time_point startTime;

        void updateState(std::chrono::high_resolution_clock::time_point currentTime);
        void updatePosition(float elapsedTime);
        void render(olc::vf2d currPosition, float elapsedTime);
        void moveTowardsTarget(const olc::vf2d& target, float speed, float elapsedTime);
        float calculateCrossingSpeed(olc::vf2d startPosition, olc::vf2d endPosition);
};

Car::Car(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, std::thread::id carId, crossingDatum crossingData) : engine(engine), startTime(startTime), carId(carId), crossingData(crossingData) {
    if(crossingData.direction == Direction::EAST){
        currPosition = WAIT_TO_GO_EAST1;
        sprite = new olc::Sprite("./source/assets/vehicles/hatchbackSports_E.png");
    } else {
        currPosition = WAIT_TO_GO_WEST1;
        sprite = new olc::Sprite("./source/assets/vehicles/hatchbackSports_W.png");
    }

    state = CarState::HIDDEN;
    decal = new olc::Decal(sprite);
}

Car::~Car() {} // todo clean up

void Car::update(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime) {
    updateState(currentTime);
    updatePosition(elapsedTime);
}

void Car::updateState(std::chrono::high_resolution_clock::time_point currentTime) {
    std::chrono::duration<double> startWaitTime = crossingData.startWaitTime;
    std::chrono::duration<double> enterTime = crossingData.enterTime;
    std::chrono::duration<double> leaveTime = crossingData.leaveTime;

    auto timeSinceStart = std::chrono::duration<double>(currentTime - startTime);

    if (timeSinceStart > startWaitTime && timeSinceStart < enterTime) {
        std::cout << "waiting" << std::endl;
        state = CarState::WAITING1;
    } else if (timeSinceStart >= enterTime) {
        std::cout << "crossing" << std::endl;
        state = CarState::CROSSING;
    } else if (timeSinceStart > leaveTime) {
        std::cout << "hidden" << std::endl;
        state = CarState::HIDDEN;
    }
}

void Car::updatePosition(float elapsedTime) {
    olc::vf2d startPosition;
    olc::vf2d targetPosition;
    float speed;

    switch(state) {
        case CarState::WAITING1:
            render(currPosition, elapsedTime);
            break;
        case CarState::WAITING2:
            targetPosition = (crossingData.direction == Direction::EAST)? WAIT_TO_GO_EAST2 : WAIT_TO_GO_WEST2;
            
            moveTowardsTarget(targetPosition, SPEED, elapsedTime);
            render(currPosition, elapsedTime);
            break;
        case CarState::CROSSING:
            startPosition = (crossingData.direction == Direction::EAST)? WAIT_TO_GO_EAST1 : WAIT_TO_GO_WEST1;
            targetPosition = (crossingData.direction == Direction::EAST)? END_CROSSING_EAST : END_CROSSING_WEST;
            speed = calculateCrossingSpeed(startPosition, targetPosition);
            
            moveTowardsTarget(targetPosition, speed, elapsedTime);
            render(currPosition, elapsedTime);
            break;
        default:
            break;
    }
}

void Car::render(olc::vf2d currPosition, float elapsedTime) {
    engine->DrawDecal(currPosition, decal);
}

void Car::moveTowardsTarget(const olc::vf2d& target, float speed, float elapsedTime) {
    olc::vf2d distanceToTarget = target - currPosition;
    olc::vf2d distanceToMove = {0.0, 0.0};

    // if(distanceToTarget.x == 0 && distanceToTarget.y == 0) {
    //     return;
    // }

    if(distanceToTarget.mag2() > 0.0) {
        distanceToMove = distanceToTarget.norm() * speed * elapsedTime;
    }

    if(distanceToTarget.mag2() <= distanceToMove.mag2()) {
        distanceToMove = distanceToTarget;
    }

    currPosition += distanceToMove;
}

float Car::calculateCrossingSpeed(olc::vf2d startPosition, olc::vf2d endPosition) {
    olc::vf2d distanceVector = endPosition - startPosition;
    auto timeDifference = crossingData.leaveTime - crossingData.enterTime;
    double elapsedTimeSeconds = std::chrono::duration<double>(timeDifference).count();
    return (elapsedTimeSeconds > 0.0) ?  distanceVector / static_cast<float>(elapsedTimeSeconds) : 0.0;
}

#endif