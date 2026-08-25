#include "MainMenu.h"
#include <Arduino.h>

// Binds this menu to the shared TFT driver.
MainMenu::MainMenu(TFT_eSPI* tftInstance)
    : tft(tftInstance), dragTarget(DRAG_NONE) {}

// Fills slider geometry for the two stacked brightness cards.
void MainMenu::layoutSliders() {
    const int16_t cardW = TFT_W - (PAD * 2);
    const int16_t trackW = cardW - 40;
    const int16_t trackH = SLIDER_TRACK_H;

    screenSlider.cardX = PAD;
    screenSlider.cardY = CONTENT_Y + PAD;
    screenSlider.cardW = cardW;
    screenSlider.cardH = MAIN_CARD_H;
    screenSlider.trackX = PAD + 20;
    screenSlider.trackY = screenSlider.cardY + 64;
    screenSlider.trackW = trackW;
    screenSlider.trackH = trackH;
    screenSlider.knobRadius = SLIDER_KNOB_R;
    screenSlider.label = "SCREEN";
    screenSlider.labelX = PAD + 44;
    screenSlider.fillColor = COLOR_SCREEN_FILL;

    lightSlider.cardX = PAD;
    lightSlider.cardY = screenSlider.cardY + MAIN_CARD_H + GAP;
    lightSlider.cardW = cardW;
    lightSlider.cardH = MAIN_CARD_H;
    lightSlider.trackX = PAD + 20;
    lightSlider.trackY = lightSlider.cardY + 64;
    lightSlider.trackW = trackW;
    lightSlider.trackH = trackH;
    lightSlider.knobRadius = SLIDER_KNOB_R;
    lightSlider.label = "LIGHT";
    lightSlider.labelX = PAD + 44;
    lightSlider.fillColor = COLOR_LIGHT_FILL;
}

// Paints one brightness card's static chrome (card, icon, title).
void MainMenu::drawBrightnessChrome(const HorizontalSlider& slider, bool isScreen) {
    UiWidgets::drawCard(tft, slider.cardX, slider.cardY, slider.cardW, slider.cardH);

    const int16_t iconX = slider.cardX + 24;
    const int16_t iconY = slider.cardY + 24;
    if (isScreen) {
        UiWidgets::drawSunIcon(tft, iconX, iconY, COLOR_SCREEN_FILL);
    } else {
        UiWidgets::drawLampIcon(tft, iconX, iconY, COLOR_LIGHT_FILL);
    }

    UiWidgets::drawSliderLabel(tft, slider);
    UiWidgets::drawStaticTrack(tft, slider);
}

// Sets default brightness values and card layout.
void MainMenu::init() {
    layoutSliders();
    screenSlider.value = 80;
    screenSlider.oldValue = 80;
    lightSlider.value = 40;
    lightSlider.oldValue = 40;
    dragTarget = DRAG_NONE;
}

// Draws both brightness cards into the content area.
void MainMenu::draw() {
    UiWidgets::clearContent(tft);
    drawBrightnessChrome(screenSlider, true);
    drawBrightnessChrome(lightSlider, false);
    UiWidgets::pushSlider(screenSlider, false);
    UiWidgets::pushValue(screenSlider);
    UiWidgets::pushSlider(lightSlider, false);
    UiWidgets::pushValue(lightSlider);
}

// Drags the screen or external-light slider from a content-area touch.
void MainMenu::handleTouch(int16_t touchX, int16_t touchY) {
    if (dragTarget == DRAG_NONE) {
        if (UiWidgets::hitTest(touchX, touchY, screenSlider.cardX, screenSlider.cardY,
                               screenSlider.cardW, screenSlider.cardH)) {
            dragTarget = DRAG_SCREEN;
        } else if (UiWidgets::hitTest(touchX, touchY, lightSlider.cardX, lightSlider.cardY,
                                      lightSlider.cardW, lightSlider.cardH)) {
            dragTarget = DRAG_LIGHT;
        } else {
            return;
        }
    }

    HorizontalSlider* slider = (dragTarget == DRAG_SCREEN) ? &screenSlider : &lightSlider;
    if (UiWidgets::handleSliderTouch(*slider, touchX)) {
        UiWidgets::pushSlider(*slider, false);
        UiWidgets::pushValue(*slider);
    }
}

// Clears the active slider drag when the finger lifts.
void MainMenu::handleRelease() {
    dragTarget = DRAG_NONE;
}
