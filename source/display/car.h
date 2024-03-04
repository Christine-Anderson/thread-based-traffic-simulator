#ifndef CAR_H
#define CAR_H

#include <vector>

#include "../lib/olcPixelGameEngine.h"
#include "../enums.h"

class Car {
    const olc::vf2d OFFSET = olc::vf2d(256, 256); 

    private:
        Direction direction; //todo need start, enter, leave stats
        olc::vf2d xy_position;
        olc::vf2d xy_velocity;
        olc::Sprite *sprite = nullptr;
        olc::Decal *decal = nullptr;
    public:
        Car(Direction direction);
        ~Car();
        void render(olc::PixelGameEngine* engine, float fElapsedTime);
};

Car::Car(Direction direction) : direction(direction) {
    xy_position = olc::vf2d(0, 350);
    xy_velocity = olc::vf2d(0, 0);
    sprite = new olc::Sprite("./source/assets/vehicles/hatchbackSports_E.png");
    decal = new olc::Decal(sprite);
}

Car::~Car() {} // todo clean up

void Car::render(olc::PixelGameEngine* engine, float fElapsedTime) {
    engine->DrawDecal(xy_position - OFFSET, decal);
}

#endif