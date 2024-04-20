#ifndef CAR_H
#define CAR_H

#include <vector>
#include <random>

#include "../lib/olcPixelGameEngine.h"
#include "../definitions.h"

class Car {
    const olc::vf2d OFFSET = {256, 256};
    const olc::vf2d START_EAST = {-256, 360};
    const olc::vf2d START_WEST = {1256, 230};
    const std::vector<olc::vf2d> WAIT_TO_GO_EAST = {{125, 360}};
    const std::vector<olc::vf2d> WAIT_TO_GO_WEST = {{800, 230}};
    const std::vector<olc::vf2d> PATH_CROSSING_EAST = {{1256, 360}};
    const std::vector<olc::vf2d> PATH_CROSSING_WEST = {{700, 360}, {350, 360}, {200, 230}, {-256, 230}};
    const float DEFAULT_SPEED = 500.0;

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
        olc::Sprite *sprite = nullptr;
        olc::Decal *decal = nullptr;
        std::chrono::duration<double> normalizeStartTimeTo;
        size_t currentPathIndex;
        std::vector<olc::vf2d> path;
        float speed;

        olc::Sprite* generateRandomCar(Direction direction);
        void updateState(std::chrono::high_resolution_clock::time_point currentTime);
        void initState(CarState state);
        void updatePosition(float elapsedTime);
        void render(olc::vf2d currPosition, float elapsedTime);
        void moveAlongPath(const std::vector<olc::vf2d>& path, float speed, float elapsedTime);
        float calculateAverageSpeedAlongPath(const std::vector<olc::vf2d>& path);
};

Car::Car(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, std::chrono::duration<double> normalizeStartTimeTo, std::thread::id carId, crossingDatum crossingData) : engine(engine), startTime(startTime), normalizeStartTimeTo(normalizeStartTimeTo), carId(carId), crossingData(crossingData) {
    if(crossingData.direction == Direction::EAST){
        currPosition = START_EAST;
        sprite = generateRandomCar(crossingData.direction);
    } else {
        currPosition = START_WEST;
        sprite = generateRandomCar(crossingData.direction);
    }
    currentPathIndex = 0;
    state = CarState::HIDDEN;
    decal = new olc::Decal(sprite);
    speed = DEFAULT_SPEED;
}

Car::~Car() {} // todo clean up

olc::Sprite* Car::generateRandomCar(Direction direction) {
    std::random_device rand;
    std::mt19937 gen(rand());
    std::uniform_int_distribution<int> distrib(1, 4);

    int randomNumber = distrib(gen);

    std::string imagePath = "./source/assets/vehicles/";

    switch (randomNumber) {
        case 1:
            imagePath += (direction == Direction::EAST) ? "hatchbackSports_E.png" : "hatchbackSports_W.png";
            break;
        case 2:
            imagePath += (direction == Direction::EAST) ? "sedan_E.png" : "sedan_W.png";
            break;
        case 3:
            imagePath += (direction == Direction::EAST) ? "truck_E.png" : "truck_W.png";
            break;
        case 4:
            imagePath += (direction == Direction::EAST) ? "van_E.png" : "van_W.png";
            break;
        default:
            imagePath += (direction == Direction::EAST) ? "hatchbackSports_E.png" : "hatchbackSports_W.png";
            break;
    }

    olc::Sprite* randomSprite = new olc::Sprite(imagePath);
    return randomSprite;
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
    switch(state) {
        case CarState::WAITING_VISIBLE:
        case CarState::CROSSING:
            moveAlongPath(path, speed, elapsedTime);
            render(currPosition, elapsedTime);
            break;
        default:
            break;
    }
}

void Car::render(olc::vf2d currPosition, float elapsedTime) {
    engine->DrawDecal(currPosition - OFFSET, decal);
}
//todo refactor below out?
void Car::moveAlongPath(const std::vector<olc::vf2d>& path, float speed, float elapsedTime) {
    if(currentPathIndex >= path.size()) {
        return;
    }

    olc::vf2d distanceToTarget = path[currentPathIndex] - currPosition;
    olc::vf2d distanceToMove = {0.0, 0.0};
    // std::cout << "distanceToTarget " << std::to_string(distanceToTarget.x) << std::to_string(distanceToTarget.y) <<std::endl;

    if(distanceToTarget.mag2() > 0.0) {
        distanceToMove = distanceToTarget.norm() * speed * elapsedTime;
        // std::cout << "distanceToMove " << std::to_string(distanceToMove.x) << std::to_string(distanceToMove.y) << std::endl;
    }

    if(distanceToTarget.mag2() <= distanceToMove.mag2()) {
        distanceToMove = distanceToTarget;
        currentPathIndex++;
    } 

    currPosition += distanceToMove;
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