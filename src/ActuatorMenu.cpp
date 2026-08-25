#include "ActuatorMenu.h"
#include <Arduino.h>

// Binds this actuator screen to the shared TFT driver.
ActuatorMenu::ActuatorMenu(TFT_eSPI* tftInstance)
    : tft(tftInstance), dragTarget(DRAG_NONE) {}

// Places the speed card and the two actuator buttons in the content area.
void ActuatorMenu::layoutControls() {
    const int16_t cardW = TFT_W - (PAD * 2);

    speedSlider.cardX = PAD;
    speedSlider.cardY = CONTENT_Y + PAD;
    speedSlider.cardW = cardW;
    speedSlider.cardH = ACT_SLIDER_CARD_H;
    speedSlider.trackX = PAD + 20;
    speedSlider.trackY = speedSlider.cardY + 58;
    speedSlider.trackW = cardW - 40;
    speedSlider.trackH = SLIDER_TRACK_H;
    speedSlider.knobRadius = SLIDER_KNOB_R;
    speedSlider.label = "SPEED";
    speedSlider.labelX = PAD + 16;
    speedSlider.fillColor = COLOR_ACCENT;

    contractBtn.x = PAD;
    contractBtn.y = ACT_BTN_Y;
    contractBtn.w = ACT_BTN_W;
    contractBtn.h = ACT_BTN_H;
    contractBtn.label = "CONTRACT";
    contractBtn.color = COLOR_BTN_CONTRACT;
    contractBtn.pressedColor = COLOR_BTN_CONTRACT_PRESSED;
    contractBtn.icon = ICON_DOWN;
    contractBtn.pressed = false;

    retractBtn.x = PAD + ACT_BTN_W + GAP;
    retractBtn.y = ACT_BTN_Y;
    retractBtn.w = ACT_BTN_W;
    retractBtn.h = ACT_BTN_H;
    retractBtn.label = "RETRACT";
    retractBtn.color = COLOR_BTN_RETRACT;
    retractBtn.pressedColor = COLOR_BTN_RETRACT_PRESSED;
    retractBtn.icon = ICON_UP;
    retractBtn.pressed = false;
}

// Redraws a button only when its pressed state changes.
void ActuatorMenu::setButtonPressed(TouchButton& btn, bool pressed) {
    if (btn.pressed == pressed) return;
    btn.pressed = pressed;
    UiWidgets::drawButton(tft, btn);
}

// Sets default speed and lays out the speed slider plus action buttons.
void ActuatorMenu::init() {
    layoutControls();
    speedSlider.value = 50;
    speedSlider.oldValue = 50;
    dragTarget = DRAG_NONE;
}

// Draws the speed card and the contract/retract hold-buttons.
void ActuatorMenu::draw() {
    UiWidgets::clearContent(tft);

    UiWidgets::drawCard(tft, speedSlider.cardX, speedSlider.cardY,
                        speedSlider.cardW, speedSlider.cardH);
    UiWidgets::drawSliderLabel(tft, speedSlider);
    UiWidgets::drawStaticTrack(tft, speedSlider);
    UiWidgets::pushSlider(speedSlider, false);
    UiWidgets::pushValue(speedSlider);

    contractBtn.pressed = false;
    retractBtn.pressed = false;
    UiWidgets::drawButton(tft, contractBtn);
    UiWidgets::drawButton(tft, retractBtn);
}

// Routes a touch to the speed slider or to one of the two hold-buttons.
void ActuatorMenu::handleTouch(int16_t touchX, int16_t touchY) {
    if (dragTarget == DRAG_NONE) {
        if (UiWidgets::hitTest(touchX, touchY, speedSlider.cardX, speedSlider.cardY,
                               speedSlider.cardW, speedSlider.cardH)) {
            dragTarget = DRAG_SPEED;
        } else if (UiWidgets::buttonContains(contractBtn, touchX, touchY)) {
            dragTarget = DRAG_CONTRACT;
        } else if (UiWidgets::buttonContains(retractBtn, touchX, touchY)) {
            dragTarget = DRAG_RETRACT;
        } else {
            return;
        }
    }

    if (dragTarget == DRAG_SPEED) {
        if (UiWidgets::handleSliderTouch(speedSlider, touchX)) {
            UiWidgets::pushSlider(speedSlider, false);
            UiWidgets::pushValue(speedSlider);
        }
        return;
    }

    if (dragTarget == DRAG_CONTRACT) {
        setButtonPressed(contractBtn, UiWidgets::buttonContains(contractBtn, touchX, touchY));
        setButtonPressed(retractBtn, false);
    } else if (dragTarget == DRAG_RETRACT) {
        setButtonPressed(retractBtn, UiWidgets::buttonContains(retractBtn, touchX, touchY));
        setButtonPressed(contractBtn, false);
    }
}

// Releases any held button and ends an in-progress slider drag.
void ActuatorMenu::handleRelease() {
    setButtonPressed(contractBtn, false);
    setButtonPressed(retractBtn, false);
    dragTarget = DRAG_NONE;
}
