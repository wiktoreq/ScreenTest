#pragma once
#include <TFT_eSPI.h>
#include "Config.h"

struct VerticalSlider {
    int16_t x;
    int16_t yMin;
    int16_t yMax;
    int16_t knobY;
    int16_t oldKnobY;    // New: Tracks previous position to erase trails
    uint8_t radius;
    uint8_t value;
    const char* label;
};

class MainMenu {
private:
    TFT_eSPI* tft;
    TFT_eSprite knobSprite; // Tiny sprite just for the moving knob

    VerticalSlider posSlider;
    VerticalSlider speedSlider;

    void drawStaticBackground();
    void renderKnobSprite();
    void updateSliderVisuals(VerticalSlider& slider);
    void updateSliderValueText(const VerticalSlider& slider);

public:
    // We no longer need the contentSprite pointer
    MainMenu(TFT_eSPI* tftInstance);
    ~MainMenu();

    void init();
    void draw(); // Renders the initial screen state
    
    void handleTouch(int16_t touchX, int16_t touchY);

    uint8_t getPosition() const { return posSlider.value; }
    uint8_t getSpeed() const { return speedSlider.value; }
};