#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION

#include <set>

#include "lib/olcPixelGameEngine.h"
#include "display/carManager.h"
#include "display/menu.h"
#include "traffic/trafficSimulator.h"
#include "definitions.h"

using namespace traffic;

class RenderSimulation : public olc::PixelGameEngine {
    public:
        RenderSimulation() {
            sAppName = "RenderSimulation";
        }

        Menu* menu;
        Statistics* simStats;
        CarManager* carManager;

    private:
        olc::Decal* roadDecal = nullptr;
        SimulationState simState;
        

	bool OnUserCreate() override {
		roadDecal = new olc::Decal(new olc::Sprite("./source/assets/road2.png"));
        menu = new Menu(this);
        simState = SimulationState::MENU;

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override {
		Clear(olc::VERY_DARK_GREY);
        DrawDecal(olc::vf2d(0.0, 0.0), roadDecal);
        MenuData menuData;

        switch(simState) {
            case SimulationState::MENU:
                menuData = menu->update();
                if (menuData.runSimulation) {
                    menuData.runSimulation = false;
                    startSimulation(menuData.threadSchedulingStrategy, menuData.numThreads);
                    simState = SimulationState::SIMULATION_RUNNING;
                }
                break;
            case SimulationState::SIMULATION_RUNNING:
                this->DrawStringDecal({20, 200}, "Waiting: " + std::to_string(carManager->getCarsWaiting(Direction::EAST)), olc::WHITE, {2.0, 2.0});
                this->DrawStringDecal({800, 200}, "Waiting: " + std::to_string(carManager->getCarsWaiting(Direction::WEST)), olc::WHITE, {2.0, 2.0});

                carManager->updateCars(std::chrono::high_resolution_clock::now(), elapsedTime);

                if (carManager->areCarsDone()) {
                    simState = SimulationState::SIMULATION_DONE;
                }
                break;
            case SimulationState::SIMULATION_DONE:
                this->DrawStringDecal({350, 300}, "Simulation complete!", olc::WHITE, {2.0, 2.0});
                this->DrawStringDecal({260, 640}, "- Press spacebar to continue -", olc::WHITE, {2.0, 2.0});
                if (GetKey(olc::Key::SPACE).bPressed) {
                    simState = SimulationState::MENU;
                }
                break;
        }
		
        this->DrawStringDecal({10, 10}, "X: " + std::to_string(GetMouseX()) + " Y: " + std::to_string(GetMouseY()), olc::WHITE, {2.0, 2.0}); //todo delete
        // this->DrawRectDecal({ static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY())}, {512, 512}, olc::RED);

		return true;
	}

    void startSimulation(Strategy threadSchedulingStrategy, int numThreads) {
        TrafficSimulator trafficSimulator(threadSchedulingStrategy, numThreads, false);
        simStats = trafficSimulator.runSimulation();
        carManager = new CarManager(this, std::chrono::high_resolution_clock::now(), simStats);
        carManager->createCars();
    }
};

int main() {
	RenderSimulation simulation;
	if (simulation.Construct(1000, 700, 1, 1))
		simulation.Start();

	return 0;
}
