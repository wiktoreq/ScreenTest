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
    // Allocates knob, percent, and button sprites (call once from setup).
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

    // Draws the static slider title; the moving knob is blitted separately.
    static void drawSliderLabel(TFT_eSPI* tft, const HorizontalSlider& slider);

    // Draws the static track and fill (no knob) as part of the screen chrome.
    static void drawStaticTrack(TFT_eSPI* tft, const HorizontalSlider& slider);

    // Blits the 34x34 knob sprite at the current slider value.
    static void pushKnob(const HorizontalSlider& slider);

    // Moves the knob with object-sized blits: restore old hole, update fill, push knob.
    static void updateSlider(const HorizontalSlider& slider);

    // Blits the percent text from a sprite sized to the readout.
    static void pushValue(const HorizontalSlider& slider);

    // Maps a touch X to 0-100 on the given slider; returns true when the value changed.
    static bool handleSliderTouch(HorizontalSlider& slider, int16_t touchX);

    // Blits one hold-button from a sprite sized to the button.
    static void pushButton(const TouchButton& btn);

    // Returns true when a point is inside the button bounds.
    static bool buttonContains(const TouchButton& btn, int16_t x, int16_t y);

private:
    static TFT_eSPI* display;
    static TFT_eSprite* knobSprite;
    static TFT_eSprite* valueSprite;
    static TFT_eSprite* buttonSprite;

    // Creates one 16-bit sprite or halts if RAM cannot be allocated.
    static TFT_eSprite* createSprite(int16_t width, int16_t height, const char* name);

    // Converts a 0-100 slider value to the knob's screen-center X.
    static int16_t knobCenterX(const HorizontalSlider& slider, uint8_t value);

    // Pushes a 34x34 track/fill patch over the previous knob position.
    static void restoreOldKnob(const HorizontalSlider& slider);

    // Paints only the fill segment that changed between oldValue and value.
    static void updateFillDelta(const HorizontalSlider& slider);
};
