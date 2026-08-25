#pragma once
#include <TFT_eSPI.h>
#include "Config.h"

enum ButtonIcon {
    ICON_NONE = 0,
    ICON_DOWN = 1,
    ICON_UP   = 2
};

struct HorizontalSlider {
    int16_t cardX;
    int16_t cardY;
    int16_t cardW;
    int16_t cardH;
    int16_t trackX;
    int16_t trackY;
    int16_t trackW;
    int16_t trackH;
    int16_t knobRadius;
    uint8_t value;
    uint8_t oldValue;
    const char* label;
    int16_t labelX;
    uint16_t fillColor;
};

struct TouchButton {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    const char* label;
    uint16_t color;
    uint16_t pressedColor;
    ButtonIcon icon;
    bool pressed;
};

class UiWidgets {
public:
    // Allocates the shared slider and percent sprites (call once from setup).
    static void init(TFT_eSPI* tft);

    // Returns true when a point lies inside the given rectangle.
    static bool hitTest(int16_t x, int16_t y, int16_t rx, int16_t ry, int16_t rw, int16_t rh);

    // Fills the content pane with the background color (leaves the tab bar alone).
    static void clearContent(TFT_eSPI* tft);

    // Draws a rounded card used as a control group background.
    static void drawCard(TFT_eSPI* tft, int16_t x, int16_t y, int16_t w, int16_t h);

    // Draws a small sun glyph used as the screen-brightness icon.
    static void drawSunIcon(TFT_eSPI* tft, int16_t cx, int16_t cy, uint16_t color);

    // Draws a small lamp glyph used as the external-light icon.
    static void drawLampIcon(TFT_eSPI* tft, int16_t cx, int16_t cy, uint16_t color);

    // Draws the static slider title; the moving track is blitted separately.
    static void drawSliderLabel(TFT_eSPI* tft, const HorizontalSlider& slider);

    // Blits the slider track, fill, and knob from a sprite (no background redraw).
    static void pushSlider(const HorizontalSlider& slider);

    // Blits the percent readout from a sprite over the reserved value slot.
    static void pushValue(const HorizontalSlider& slider);

    // Maps a touch X to 0-100 on the given slider; returns true when the value changed.
    static bool handleSliderTouch(HorizontalSlider& slider, int16_t touchX);

    // Paints a large rounded hold-button with optional chevron icon.
    static void drawButton(TFT_eSPI* tft, const TouchButton& btn);

    // Returns true when a point is inside the button bounds.
    static bool buttonContains(const TouchButton& btn, int16_t x, int16_t y);

private:
    static TFT_eSPI* display;
    static TFT_eSprite* sliderSprite;
    static TFT_eSprite* valueSprite;

    // Creates one 16-bit sprite or halts if RAM cannot be allocated.
    static TFT_eSprite* createSprite(int16_t width, int16_t height, const char* name);
};
