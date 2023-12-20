#pragma once

#include "lib/olcPixelGameEngine.h"
#include <vector>

class Square
{
// using MemberFunctionPtr = void (Input::*)() const;
// typedef void (Square::*MemberFunctionPtr)() const;

private:
    std::vector<olc::vf2d> get_transformed_coords();
public:
    olc::vf2d xy_position;
    olc::vf2d xy_velocity;
    int x_width;
    int y_height;
    float angle;
	olc::Sprite *sprite = nullptr;
	olc::Decal  *decal = nullptr;
    Square();
    ~Square();
    void rotate(float);
    void move(olc::vf2d);
    void init(std::string, olc::vf2d, olc::vf2d, int, int, float);
    void render(olc::PixelGameEngine*, float);
    bool check_collision(Square);
};

void Square::init(std::string filename, olc::vf2d _xy_position, olc::vf2d _xy_velocity, int _x_width, int _y_height, float _angle) {
    xy_position = _xy_position;
    xy_velocity = _xy_velocity; 
    x_width = _x_width;
    y_height = _y_height;
    angle = _angle;
    sprite = new olc::Sprite(filename);
    decal = new olc::Decal(sprite);
}

void Square::render(olc::PixelGameEngine*  engine, float fElapsedTime) {
    olc::vf2d center = olc::vf2d((float) x_width/2.0, (float) y_height/2.0);
    // engine->DrawRotatedDecal(xy_position, decal, angle, center);
    engine->DrawRotatedDecal(xy_position, decal, angle);
}

Square::Square() { }
Square::~Square() { }

void Square::rotate(float delta_angle) {
    angle += delta_angle; 
}

void Square::move(olc::vf2d delta_distance) {
    xy_position += delta_distance;
}

bool Square::check_collision(Square other_square) {
    return xy_position.x            < other_square.xy_position.x + other_square.x_width
        && xy_position.x + x_width  > other_square.xy_position.x
        && xy_position.y            < other_square.xy_position.y + other_square.y_height
        && xy_position.y + y_height > other_square.xy_position.y;
}

std::vector<olc::vf2d> get_transformed_coords() {
    std::vector<olc::vf2d> coords;
    return coords;
}




