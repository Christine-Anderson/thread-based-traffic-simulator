#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION

#include "lib/olcPixelGameEngine.h"
#include "square.h"
#include "math.h"
#include "traffic/trafficSimulator.h"

using namespace traffic;

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

public:
	Square sq_burg;
	Square sq_blue;
	float time = 0.0;
	olc::vf2d UP = olc::vf2d(0.0, -1.0);
	olc::vf2d DOWN = olc::vf2d(0.0, 1.0);
	olc::vf2d LEFT = olc::vf2d(-1.0, 0.0);
	olc::vf2d RIGHT = olc::vf2d(1.0, 0.0);

	bool OnUserCreate() override
	{
		sq_burg.init("./source/assets/burgandy.png", olc::vf2d(0.0, 20.0), olc::vf2d(0.0, 0.0), 100, 100, 0.0);
		sq_blue.init("./source/assets/blue.png", olc::vf2d(300.0, 300.0), olc::vf2d(0.0, 0.0), 100, 100, 0.0);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::VERY_DARK_GREY);
		handleInputs(fElapsedTime);
		sq_burg.render(this, fElapsedTime);
		sq_blue.render(this, fElapsedTime);
		return true;
	}

	olc::vf2d opposite(olc::vf2d direction) {
		return direction * olc::vf2d(-1.0, -1.0);
	}


	void handleInputs(float fElapsedTime) {
		float angle_increment = 0;
		olc::vf2d the_move = olc::vf2d(0.0, 0.0);

		if (GetKey(olc::Key::W).bHeld || GetKey(olc::Key::W).bPressed) {
			the_move += UP;
		}
		if (GetKey(olc::Key::S).bHeld || GetKey(olc::Key::S).bPressed) {
			the_move += DOWN;
		}
		if (GetKey(olc::Key::A).bHeld || GetKey(olc::Key::A).bPressed) {
			the_move += LEFT;
		}
		if (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::D).bPressed) {
			the_move += RIGHT;
		}
		if (GetKey(olc::Key::E).bHeld || GetKey(olc::Key::E).bPressed) {
			angle_increment = M_PI * 0.001;
		}

		sq_blue.move(the_move);
		sq_blue.rotate(angle_increment);
		if(sq_blue.check_collision(sq_burg)) {
			sq_blue.move(opposite(the_move));
			sq_blue.rotate(-1 * angle_increment);
		}
		

		angle_increment = 0;
		the_move = olc::vf2d(0.0, 0.0);
		if (GetKey(olc::Key::I).bHeld || GetKey(olc::Key::I).bPressed) {
			the_move += UP;
		}
		if (GetKey(olc::Key::K).bHeld || GetKey(olc::Key::K).bPressed) {
			the_move += DOWN;
		}
		if (GetKey(olc::Key::J).bHeld || GetKey(olc::Key::J).bPressed) {
			the_move += LEFT;
		}
		if (GetKey(olc::Key::L).bHeld || GetKey(olc::Key::L).bPressed) {
			the_move += RIGHT;
		}
		if (GetKey(olc::Key::O).bHeld || GetKey(olc::Key::O).bPressed) {
			angle_increment = M_PI * 0.001;
		}

		sq_burg.move(the_move);
		sq_burg.rotate(angle_increment);
		if(sq_burg.check_collision(sq_blue)) {
			sq_burg.move(opposite(the_move));
			sq_burg.rotate(-1 * angle_increment);
		}
	}
};


int main()
{
	// Example demo;
	// if (demo.Construct(1000, 1000, 1, 1))
	// 	demo.Start();
	TrafficSimulator trafficSimulator(5, 5, false, false);
	trafficSimulator.runSimulation();

	return 0;
}
