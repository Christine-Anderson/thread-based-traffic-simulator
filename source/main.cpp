#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION

#include <set>

#include "lib/olcPixelGameEngine.h"
#include "display/carManager.h"
#include "traffic/trafficSimulator.h"
#include "definitions.h"

using namespace traffic;

class RenderSimulation : public olc::PixelGameEngine {
    public:
        RenderSimulation(Statistics* simStats) : simStats(simStats) {
            sAppName = "RenderSimulation";
        }

        Statistics* simStats;
        CarManager* carManager;
        olc::Sprite* roadSprite = nullptr;
        olc::Decal* roadDecal = nullptr;

	bool OnUserCreate() override {
		roadSprite = new olc::Sprite("./source/assets/road.png");
		roadDecal = new olc::Decal(roadSprite);

        carManager = new CarManager(this, std::chrono::high_resolution_clock::now(), simStats);
        carManager->createCars();

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override {
		Clear(olc::VERY_DARK_GREY);
		DrawDecal(olc::vf2d(0.0, 0.0), roadDecal);

        this->DrawStringDecal({10, 10}, "X: " + std::to_string(GetMouseX()) + " Y: " + std::to_string(GetMouseY()), olc::WHITE, {2.0, 2.0}); //todo delete
        // this->DrawRectDecal({ static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY())}, {512, 512}, olc::RED);
        this->DrawStringDecal({20, 150}, "Waiting: " + std::to_string(carManager->getCarsWaiting(Direction::EAST)), olc::WHITE, {2.0, 2.0});
        this->DrawStringDecal({800, 150}, "Waiting: " + std::to_string(carManager->getCarsWaiting(Direction::WEST)), olc::WHITE, {2.0, 2.0});

        carManager->updateCars(std::chrono::high_resolution_clock::now(), elapsedTime);

        if (carManager->areCarsDone()) {
            this->DrawStringDecal({370,300}, "Simulation complete!", olc::WHITE, {2.0, 2.0});
        }

		return true;
	}

	void handleInputs(float elapsedTime) {
	// 	float angle_increment = 0;
	// 	olc::vf2d the_move = olc::vf2d(0.0, 0.0);

	// 	if (GetKey(olc::Key::W).bHeld || GetKey(olc::Key::W).bPressed) {
	// 		the_move += UP;
	// 	}
	// 	if (GetKey(olc::Key::S).bHeld || GetKey(olc::Key::S).bPressed) {
	// 		the_move += DOWN;
	// 	}
	// 	if (GetKey(olc::Key::A).bHeld || GetKey(olc::Key::A).bPressed) {
	// 		the_move += LEFT;
	// 	}
	// 	if (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::D).bPressed) {
	// 		the_move += RIGHT;
	// 	}
	// 	if (GetKey(olc::Key::E).bHeld || GetKey(olc::Key::E).bPressed) {
	// 		angle_increment = M_PI * 0.001;
	// 	}

	// 	sq_blue.move(the_move);
	// 	sq_blue.rotate(angle_increment);
	// 	if(sq_blue.check_collision(sq_burg)) {
	// 		sq_blue.move(opposite(the_move));
	// 		sq_blue.rotate(-1 * angle_increment);
	// 	}
		

	// 	angle_increment = 0;
	// 	the_move = olc::vf2d(0.0, 0.0);
	// 	if (GetKey(olc::Key::I).bHeld || GetKey(olc::Key::I).bPressed) {
	// 		the_move += UP;
	// 	}
	// 	if (GetKey(olc::Key::K).bHeld || GetKey(olc::Key::K).bPressed) {
	// 		the_move += DOWN;
	// 	}
	// 	if (GetKey(olc::Key::J).bHeld || GetKey(olc::Key::J).bPressed) {
	// 		the_move += LEFT;
	// 	}
	// 	if (GetKey(olc::Key::L).bHeld || GetKey(olc::Key::L).bPressed) {
	// 		the_move += RIGHT;
	// 	}
	// 	if (GetKey(olc::Key::O).bHeld || GetKey(olc::Key::O).bPressed) {
	// 		angle_increment = M_PI * 0.001;
	// 	}

	// 	sq_burg.move(the_move);
	// 	sq_burg.rotate(angle_increment);
	// 	if(sq_burg.check_collision(sq_blue)) {
	// 		sq_burg.move(opposite(the_move));
	// 		sq_burg.rotate(-1 * angle_increment);
	// 	}
	}
};


int main() {
	TrafficSimulator trafficSimulator(5, 5, false, false);
	Statistics* simStats = trafficSimulator.runSimulation();

	RenderSimulation simulation(simStats);
	if (simulation.Construct(1000, 700, 1, 1))
		simulation.Start();

	return 0;
}
