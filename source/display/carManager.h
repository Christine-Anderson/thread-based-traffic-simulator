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
        // void renderCars();

        void incrementCarsWaiting(Direction direction);
        void decrementCarsWaiting(Direction direction);
        int getCarsWaiting(Direction direction);

        // void updateWaitingStates();

    private:
        olc::PixelGameEngine* engine;
        std::chrono::high_resolution_clock::time_point startTime;
        Statistics* simStats;
        int carsWaitingEast;
        int carsWaitingWest;
        std::vector<Car*> cars;
        // void handleWaitingVisibleState();
        // void handleWaitingHiddenState();
};

CarManager::CarManager(olc::PixelGameEngine* engine, std::chrono::high_resolution_clock::time_point startTime, Statistics* simStats) : engine(engine), startTime(startTime), simStats(simStats), carsWaitingEast(0), carsWaitingWest(0) {}

CarManager::~CarManager() {} // todo clean up

void CarManager::createCars() {
    const std::vector<std::thread::id>& crossingOrder = simStats->getCrossingOrder();
    const std::unordered_map<std::thread::id, crossingDatum>& crossingData = simStats->getCrossingData();
    const std::chrono::duration<double> normalizeStartTimeTo = crossingData.at(crossingOrder[0]).startWaitTime;

    for (auto& threadId : crossingOrder) {  
        crossingDatum data = crossingData.at(threadId);
        Car* car = new Car(engine, startTime, normalizeStartTimeTo, threadId, data);
        cars.push_back(car);
    }
}

void CarManager::updateCars(std::chrono::high_resolution_clock::time_point currentTime, float elapsedTime) {
    // std::set<CarState> carStates;
    std::cout << "we made it" << std::endl;
    for (auto& car : cars) {
        std::cout << "lopp" << std::endl;
        car->update(currentTime, elapsedTime);
        // carStates.insert(car->getState());
    }
}

bool CarManager::areCarsDone() { //todo 
    // bool allDone = true;
    // for (const auto& state : carStates) {
    //     if (state != CarState::DONE) {
    //         allDone = false;
    //         break;
    //     }
    // }

    // return allDone;

    return false;
}

void CarManager::incrementCarsWaiting(Direction direction) {
    switch (direction){
        case Direction::EAST:
            carsWaitingEast++;
            break;
        case Direction::WEST:
            carsWaitingWest++;
            break;
        default:
            break;
    }
}

void CarManager::decrementCarsWaiting(Direction direction) {
    switch (direction){
        case Direction::EAST:
            carsWaitingEast--;
            break;
        case Direction::WEST:
            carsWaitingWest--;
            break;
        default:
            break;
    }
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