#ifndef MENU_H
#define MENU_H

#include "../lib/olcPixelGameEngine.h"
#include "../definitions.h"

class Menu {
    const std::string MENU_PATH = "./source/assets/menu/";
    
    const olc::vf2d MENU_POSITION = {167, 133};

    const olc::vf2d FIRST_COME_FIRST_SERVED_BUTTON_POSITION = olc::vf2d(298, 164) + MENU_POSITION;
    const olc::vf2d TIME_BASED_PREEMPTION_BUTTON_POSITION = olc::vf2d(458, 164) + MENU_POSITION;
    const olc::vf2d FIVE_BUTTON_POSITION = olc::vf2d(298, 260) + MENU_POSITION;
    const olc::vf2d TEN_BUTTON_POSITION = olc::vf2d(394, 260) + MENU_POSITION;
    const olc::vf2d TWENTY_BUTTON_POSITION = olc::vf2d(490, 260) + MENU_POSITION;
    const olc::vf2d START_BUTTON_POSITION = olc::vf2d(282, 361) + MENU_POSITION;

    const olc::vf2d SMALL_BUTTON_SIZE = {74, 41};
    const olc::vf2d LARGE_BUTTON_SIZE = {105, 41};

    public:
        Menu(olc::PixelGameEngine* engine);
        ~Menu();
        MenuData update();

    private:
        olc::PixelGameEngine* engine;
        olc::Decal* menuDecal = nullptr;
        olc::Decal* strategyBorderDecal = nullptr;
        olc::Decal* numberBorderDecal = nullptr;
        MenuData data;
        olc::vf2d strategyButton;
        olc::vf2d numThreadsButton;

        void handleUserInput();
        bool buttonClicked(int mouseX, int mouseY, olc::vf2d buttonPosition, olc::vf2d buttonSize);
        void render();
};

Menu::Menu(olc::PixelGameEngine* engine) : engine(engine) {
    menuDecal = new olc::Decal(new olc::Sprite(MENU_PATH + "menu.png"));
    strategyBorderDecal = new olc::Decal(new olc::Sprite(MENU_PATH + "strategy_border.png"));
    numberBorderDecal = new olc::Decal(new olc::Sprite(MENU_PATH + "number_border.png"));

    strategyButton = FIRST_COME_FIRST_SERVED_BUTTON_POSITION;
    numThreadsButton = FIVE_BUTTON_POSITION;

    data.runSimulation = false;
    data.threadSchedulingStrategy = Strategy::FIRST_COME_FIRST_SERVED;
    data.numThreads = 5;
}

Menu::~Menu() {
    delete menuDecal;
}

MenuData Menu::update() {
    handleUserInput();
    render();
    return data;
}

void Menu::handleUserInput() {
    int mouseX = engine->GetMouseX();
    int mouseY = engine->GetMouseY();
    bool mouseClicked = engine->GetMouse(0).bPressed;

    data.runSimulation = false;

    if (mouseClicked && buttonClicked(mouseX, mouseY, FIRST_COME_FIRST_SERVED_BUTTON_POSITION, LARGE_BUTTON_SIZE)) {
        data.threadSchedulingStrategy = Strategy::FIRST_COME_FIRST_SERVED;
        strategyButton = FIRST_COME_FIRST_SERVED_BUTTON_POSITION;
    } else if (mouseClicked && buttonClicked(mouseX, mouseY, TIME_BASED_PREEMPTION_BUTTON_POSITION, LARGE_BUTTON_SIZE)) {
        data.threadSchedulingStrategy = Strategy::TIME_BASED_PREEMPTION;
        strategyButton = TIME_BASED_PREEMPTION_BUTTON_POSITION;
    } else if (mouseClicked && buttonClicked(mouseX, mouseY, FIVE_BUTTON_POSITION, SMALL_BUTTON_SIZE)) {
        data.numThreads = 5;
        numThreadsButton = FIVE_BUTTON_POSITION;
    } else if (mouseClicked && buttonClicked(mouseX, mouseY, TEN_BUTTON_POSITION, SMALL_BUTTON_SIZE)) {
        data.numThreads = 10;
        numThreadsButton = TEN_BUTTON_POSITION;
    } else if (mouseClicked && buttonClicked(mouseX, mouseY, TWENTY_BUTTON_POSITION, SMALL_BUTTON_SIZE)) {
        data.numThreads = 20;
        numThreadsButton = TWENTY_BUTTON_POSITION;
    } else if (mouseClicked && buttonClicked(mouseX, mouseY, START_BUTTON_POSITION, LARGE_BUTTON_SIZE)) {
        data.runSimulation = true;
    }
}

bool Menu::buttonClicked(int mouseX, int mouseY, olc::vf2d buttonPosition, olc::vf2d buttonSize) {
    bool mouseClickInBounds = mouseX >= buttonPosition.x && mouseX <= buttonPosition.x + buttonSize.x && mouseY >= buttonPosition.y && mouseY <= buttonPosition.y + buttonSize.y;
    return mouseClickInBounds;
}

void Menu::render() {
    engine->DrawDecal(MENU_POSITION, menuDecal);
    engine->DrawDecal(strategyButton, strategyBorderDecal);
    engine->DrawDecal(numThreadsButton, numberBorderDecal);
    if (data.runSimulation) {
        engine->DrawDecal(START_BUTTON_POSITION, strategyBorderDecal);
    }
}

#endif