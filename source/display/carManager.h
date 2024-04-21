#ifndef CAR_MANAGER_H
#define CAR_MANAGER_H

#include <vector>
#include <memory>

#include "car.h"
#include "../traffic/statistics.h"
#include "../definitions.h"

using namespace traffic;

class CarManager {
    const float DEFAULT_SPEED = 100.0;

    public:
        CarManager(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, Statistics* simStats);
        ~CarManager();

        void createCars();
        void updateCars(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime);
        bool areCarsDone();
        int getCarsWaiting(Direction direction);

    private:
        olc::PixelGameEngine* engine;
        std::chrono::high_resolution_clock::time_point startTime;
        Statistics* simStats;
        int carsWaitingEast;
        int carsWaitingWest;
        bool areCarsDoneEast;
        bool areCarsDoneWest;
        std::vector<Car*> carsWest;
        std::vector<Car*> carsEast;

        void updateCarsPerDirection(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime, std::vector<Car*>& cars);
        void updateCarsWaiting(CarState prevState, CarState currState, Direction direction);
        void incrementCarsWaiting(Direction direction);
        void decrementCarsWaiting(Direction direction);
};

CarManager::CarManager(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, Statistics* simStats)
    : engine(engine), startTime(startTime), simStats(simStats), carsWaitingEast(0), carsWaitingWest(0), areCarsDoneEast(false), areCarsDoneWest(false) {}

CarManager::~CarManager() {} // todo clean up

void CarManager::createCars() {
    const std::vector<std::thread::id>& crossingOrder = simStats->getCrossingOrder();
    const std::unordered_map<std::thread::id, crossingDatum>& crossingData = simStats->getCrossingData();
    const std::chrono::duration<double> normalizeStartTimeTo = crossingData.at(crossingOrder[0]).startWaitTime;

    for (auto& threadId : crossingOrder) {  
        crossingDatum data = crossingData.at(threadId);
        Car* car = new Car(engine, startTime, normalizeStartTimeTo, threadId, data);
        data.direction == Direction::EAST? carsEast.push_back(car) : carsWest.push_back(car);
    }
}

void CarManager::updateCars(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime) {
    updateCarsPerDirection(currentTime, elapsedTime, carsEast);
    updateCarsPerDirection(currentTime, elapsedTime, carsWest);
}

void CarManager::updateCarsPerDirection(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime, std::vector<Car*>& cars) {
    int numCars = cars.size();
    int firstWaitingHiddenCarIndex = -1;
    int carsWaiting = 0;
    int numCarsDone = 0;
    bool isCarWaitingVisible = false;

    for (size_t i = 0; i < cars.size(); i++) {
        cars[i]->update(currentTime, elapsedTime);

        Direction direction = cars[i]->getDirection();
        CarState state = cars[i]->getState();

        if (state == WAITING_HIDDEN || state == WAITING_VISIBLE) { //todo refactor
            carsWaiting++;
        }
        
        if (state == CarState::DONE) {
            numCarsDone++;
        } else if (state == CarState::WAITING_HIDDEN && firstWaitingHiddenCarIndex == -1) {
            firstWaitingHiddenCarIndex = i;
        } else if (state == CarState::WAITING_VISIBLE) {
            isCarWaitingVisible = true;
        }

        if (numCarsDone == cars.size()) {
            direction == Direction::EAST? areCarsDoneEast = true : areCarsDoneWest = true;
        }

        direction == Direction::EAST? carsWaitingEast = carsWaiting : carsWaitingWest = carsWaiting;
    }

    if (!isCarWaitingVisible && firstWaitingHiddenCarIndex >= 0 && firstWaitingHiddenCarIndex < cars.size()) {
        cars[firstWaitingHiddenCarIndex]->setState(CarState::WAITING_VISIBLE);
    }
}

bool CarManager::areCarsDone() { 
    return areCarsDoneEast && areCarsDoneWest;
}

int CarManager::getCarsWaiting(Direction direction) {
    switch (direction){
        case Direction::EAST:
            return carsWaitingEast;
        case Direction::WEST:
            return carsWaitingWest;
        default:
            return 0;
    }
}

#endif