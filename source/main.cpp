#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION

#include "lib/olcPixelGameEngine.h"
#include "display/car.h"
#include "traffic/trafficSimulator.h"
#include "definitions.h"

using namespace traffic;


class RenderSimulation : public olc::PixelGameEngine {
    public:
        RenderSimulation(Statistics* simStats) : simStats(simStats) {
            sAppName = "RenderSimulation";
        }

    public:
        Statistics* simStats;
        olc::Sprite* roadSprite = nullptr;
        olc::Decal* roadDecal = nullptr;
        std::vector<Car*> cars;

	bool OnUserCreate() override {
		roadSprite = new olc::Sprite("./source/assets/road.png");
		roadDecal = new olc::Decal(roadSprite);
        std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

        const std::vector<std::thread::id>& crossingOrder = simStats->getCrossingOrder();
        const std::unordered_map<std::thread::id, crossingDatum>& crossingData = simStats->getCrossingData();

        for (auto& threadId : crossingOrder) {            
            crossingDatum data = crossingData.at(threadId);
            Car* car = new Car(this, startTime, threadId, data);
            cars.push_back(car);
        }

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override {
		Clear(olc::VERY_DARK_GREY);
		DrawDecal(olc::vf2d(0.0, 0.0), roadDecal);
        this->DrawStringDecal({1,1}, "X: " + std::to_string(GetMouseX()) + " Y: " + std::to_string(GetMouseY()));

        std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
        for(auto& car : cars) {
            car->update(currentTime, elapsedTime);
        }

		return true;
	}

	olc::vf2d opposite(olc::vf2d direction) {
		return direction* olc::vf2d(-1.0, -1.0);
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
