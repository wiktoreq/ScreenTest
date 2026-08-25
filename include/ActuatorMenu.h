#pragma once
#include <TFT_eSPI.h>
#include "Screen.h"
#include "UiWidgets.h"

class ActuatorMenu : public Screen {
private:
    TFT_eSPI* tft;
    HorizontalSlider speedSlider;
    TouchButton contractBtn;
    TouchButton retractBtn;

    enum DragTarget {
        DRAG_NONE = 0,
        DRAG_SPEED = 1,
        DRAG_CONTRACT = 2,
        DRAG_RETRACT = 3
    };
    DragTarget dragTarget;

    // Places the speed card and the two actuator buttons in the content area.
    void layoutControls();

    // Redraws a button only when its pressed state changes.
    void setButtonPressed(TouchButton& btn, bool pressed);

public:
    // Binds this actuator screen to the shared TFT driver.
    ActuatorMenu(TFT_eSPI* tftInstance);

    // Sets default speed and lays out the speed slider plus action buttons.
    void init() override;

    // Draws the speed card and the contract/retract hold-buttons.
    void draw() override;

    // Routes a touch to the speed slider or to one of the two hold-buttons.
    void handleTouch(int16_t touchX, int16_t touchY) override;

    // Releases any held button and ends an in-progress slider drag.
    void handleRelease() override;

    // Returns the actuator movement speed (0-100).
    uint8_t getSpeed() const { return speedSlider.value; }

    // Returns true while the contract button is held.
    bool isContractPressed() const { return contractBtn.pressed; }

    // Returns true while the retract button is held.
    bool isRetractPressed() const { return retractBtn.pressed; }
};
