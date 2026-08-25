#pragma once
#include <TFT_eSPI.h>
#include "Screen.h"
#include "UiWidgets.h"

class MainMenu : public Screen {
private:
    TFT_eSPI* tft;
    HorizontalSlider screenSlider;
    HorizontalSlider lightSlider;

    enum DragTarget {
        DRAG_NONE = 0,
        DRAG_SCREEN = 1,
        DRAG_LIGHT = 2
    };
    DragTarget dragTarget;

    // Fills slider geometry for the two stacked brightness cards.
    void layoutSliders();

    // Paints one brightness card including icon, title, and slider.
    void drawBrightnessCard(const HorizontalSlider& slider, bool isScreen);

public:
    // Binds this menu to the shared TFT driver.
    MainMenu(TFT_eSPI* tftInstance);

    // Sets default brightness values and card layout.
    void init() override;

    // Draws both brightness cards into the content area.
    void draw() override;

    // Drags the screen or external-light slider from a content-area touch.
    void handleTouch(int16_t touchX, int16_t touchY) override;

    // Clears the active slider drag when the finger lifts.
    void handleRelease() override;

    // Returns the current screen-brightness value (0-100).
    uint8_t getScreenBrightness() const { return screenSlider.value; }

    // Returns the current external-light brightness value (0-100).
    uint8_t getLightBrightness() const { return lightSlider.value; }
};
