#include "UiWidgets.h"
#include "consolab24.h"
#include <Arduino.h>

TFT_eSPI* UiWidgets::display = nullptr;
TFT_eSprite* UiWidgets::knobSprite = nullptr;
TFT_eSprite* UiWidgets::valueSprite = nullptr;
TFT_eSprite* UiWidgets::buttonSprite = nullptr;

// Creates one 16-bit sprite or halts if RAM cannot be allocated.
TFT_eSprite* UiWidgets::createSprite(int16_t width, int16_t height, const char* name) {
    TFT_eSprite* sprite = new TFT_eSprite(display);
    sprite->setColorDepth(16);
    if (!sprite->createSprite(width, height)) {
        Serial.printf("Error: not enough RAM for %s sprite (%dx%d)\n", name, width, height);
        while (true) {
            delay(100);
        }
    }
    return sprite;
}

// Converts a 0-100 slider value to the knob's screen-center X.
int16_t UiWidgets::knobCenterX(const HorizontalSlider& slider, uint8_t value) {
    return slider.trackX + map(value, 0, 100, 0, slider.trackW);
}

// Allocates knob, percent, and button sprites (call once from setup).
void UiWidgets::init(TFT_eSPI* tft) {
    if (display != nullptr) {
        return;
    }
    display = tft;
    knobSprite = createSprite(KNOB_SPRITE, KNOB_SPRITE, "knob");
    valueSprite = createSprite(VALUE_SPRITE_W, VALUE_SPRITE_H, "value");
    buttonSprite = createSprite(ACT_BTN_W, ACT_BTN_H, "button");
}

// Returns true when a point lies inside the given rectangle.
bool UiWidgets::hitTest(int16_t x, int16_t y, int16_t rx, int16_t ry, int16_t rw, int16_t rh) {
    return (x >= rx && x < (rx + rw) && y >= ry && y < (ry + rh));
}

// Fills the content pane with the background color (leaves the tab bar alone).
void UiWidgets::clearContent(TFT_eSPI* tft) {
    tft->fillRect(0, CONTENT_Y, TFT_W, CONTENT_H, COLOR_BG);
}

// Draws a rounded card used as a control group background.
void UiWidgets::drawCard(TFT_eSPI* tft, int16_t x, int16_t y, int16_t w, int16_t h) {
    tft->fillRoundRect(x, y, w, h, CARD_RADIUS, COLOR_SURFACE);
    tft->drawRoundRect(x, y, w, h, CARD_RADIUS, COLOR_TRACK);
}

// Draws a small sun glyph used as the screen-brightness icon.
void UiWidgets::drawSunIcon(TFT_eSPI* tft, int16_t cx, int16_t cy, uint16_t color) {
    tft->fillCircle(cx, cy, 7, color);
    tft->drawLine(cx, cy - 14, cx, cy - 10, color);
    tft->drawLine(cx, cy + 10, cx, cy + 14, color);
    tft->drawLine(cx - 14, cy, cx - 10, cy, color);
    tft->drawLine(cx + 10, cy, cx + 14, cy, color);
    tft->drawLine(cx - 10, cy - 10, cx - 7, cy - 7, color);
    tft->drawLine(cx + 7, cy + 7, cx + 10, cy + 10, color);
    tft->drawLine(cx - 10, cy + 10, cx - 7, cy + 7, color);
    tft->drawLine(cx + 7, cy - 7, cx + 10, cy - 10, color);
}

// Draws a small lamp glyph used as the external-light icon.
void UiWidgets::drawLampIcon(TFT_eSPI* tft, int16_t cx, int16_t cy, uint16_t color) {
    tft->fillCircle(cx, cy - 3, 8, color);
    tft->fillTriangle(cx - 8, cy - 1, cx + 8, cy - 1, cx, cy + 9, color);
    tft->fillRect(cx - 4, cy + 8, 8, 4, color);
    tft->drawFastHLine(cx - 6, cy + 13, 12, color);
}

// Draws the static slider title; the moving knob is blitted separately.
void UiWidgets::drawSliderLabel(TFT_eSPI* tft, const HorizontalSlider& slider) {
    if (slider.label == nullptr) {
        return;
    }
    tft->loadFont(consolab24);
    tft->setTextDatum(TL_DATUM);
    tft->setTextColor(COLOR_TEXT, COLOR_SURFACE);
    tft->drawString(slider.label, slider.labelX, slider.cardY + 12);
    tft->unloadFont();
}

// Draws the static track and fill (no knob) as part of the screen chrome.
void UiWidgets::drawStaticTrack(TFT_eSPI* tft, const HorizontalSlider& slider) {
    const int16_t radius = slider.trackH / 2;
    tft->fillRoundRect(slider.trackX, slider.trackY, slider.trackW, slider.trackH,
                       radius, COLOR_TRACK);

    int16_t fillW = map(slider.value, 0, 100, 0, slider.trackW);
    if (fillW < slider.trackH) {
        fillW = (slider.value == 0) ? 0 : slider.trackH;
    }
    if (fillW > 0) {
        tft->fillRoundRect(slider.trackX, slider.trackY, fillW, slider.trackH,
                           radius, slider.fillColor);
    }
}

// Blits the 34x34 knob sprite at the current slider value.
void UiWidgets::pushKnob(const HorizontalSlider& slider) {
    const int16_t cx = slider.knobRadius + 1;
    const int16_t cy = slider.knobRadius + 1;

    knobSprite->fillSprite(KNOB_CHROMA);
    knobSprite->fillCircle(cx, cy, slider.knobRadius, COLOR_KNOB);
    knobSprite->drawCircle(cx, cy, slider.knobRadius, slider.fillColor);
    knobSprite->fillCircle(cx, cy, 6, slider.fillColor);

    const int16_t screenX = knobCenterX(slider, slider.value) - cx;
    const int16_t screenY = slider.trackY + (slider.trackH / 2) - cy;
    knobSprite->pushSprite(screenX, screenY, KNOB_CHROMA);
}

// Pushes a 34x34 track/fill patch over the previous knob position.
void UiWidgets::restoreOldKnob(const HorizontalSlider& slider) {
    const int16_t size = KNOB_SPRITE;
    const int16_t oldX = knobCenterX(slider, slider.oldValue);
    const int16_t spriteX = oldX - slider.knobRadius - 1;
    const int16_t spriteY = slider.trackY + (slider.trackH / 2) - slider.knobRadius - 1;
    const int16_t trackY = (size - slider.trackH) / 2;
    const int16_t trackInSpriteX = slider.trackX - spriteX;

    knobSprite->fillSprite(COLOR_SURFACE);
    knobSprite->fillRect(trackInSpriteX, trackY, slider.trackW, slider.trackH, COLOR_TRACK);

    int16_t fillW = map(slider.value, 0, 100, 0, slider.trackW);
    if (fillW < slider.trackH) {
        fillW = (slider.value == 0) ? 0 : slider.trackH;
    }
    if (fillW > 0) {
        knobSprite->fillRect(trackInSpriteX, trackY, fillW, slider.trackH, slider.fillColor);
    }

    knobSprite->pushSprite(spriteX, spriteY);
}

// Paints only the fill segment that changed between oldValue and value.
void UiWidgets::updateFillDelta(const HorizontalSlider& slider) {
    const int16_t oldFillX = knobCenterX(slider, slider.oldValue);
    const int16_t newFillX = knobCenterX(slider, slider.value);
    if (oldFillX == newFillX) {
        return;
    }

    if (newFillX > oldFillX) {
        display->fillRect(oldFillX, slider.trackY, newFillX - oldFillX, slider.trackH,
                          slider.fillColor);
    } else {
        display->fillRect(newFillX, slider.trackY, oldFillX - newFillX, slider.trackH,
                          COLOR_TRACK);
    }
}

// Moves the knob with object-sized blits: restore old hole, update fill, push knob.
void UiWidgets::updateSlider(const HorizontalSlider& slider) {
    restoreOldKnob(slider);
    updateFillDelta(slider);
    pushKnob(slider);
}

// Blits the percent text from a sprite sized to the readout.
void UiWidgets::pushValue(const HorizontalSlider& slider) {
    char valBuf[8];
    snprintf(valBuf, sizeof(valBuf), "%d%%", slider.value);

    valueSprite->fillSprite(COLOR_SURFACE);
    valueSprite->setTextDatum(MC_DATUM);
    valueSprite->setTextColor(COLOR_TEXT, COLOR_SURFACE);
    valueSprite->drawString(valBuf, VALUE_SPRITE_W / 2, VALUE_SPRITE_H / 2, 4);

    const int16_t screenX = slider.cardX + slider.cardW - 14 - VALUE_SPRITE_W;
    const int16_t screenY = slider.cardY + 12;
    valueSprite->pushSprite(screenX, screenY);
}

// Maps a touch X to 0-100 on the given slider; returns true when the value changed.
bool UiWidgets::handleSliderTouch(HorizontalSlider& slider, int16_t touchX) {
    int16_t clampedX = constrain(touchX, slider.trackX, slider.trackX + slider.trackW);
    uint8_t newValue = (uint8_t)map(clampedX, slider.trackX, slider.trackX + slider.trackW, 0, 100);

    if (newValue == slider.value) {
        return false;
    }

    slider.oldValue = slider.value;
    slider.value = newValue;
    return true;
}

// Blits one hold-button from a sprite sized to the button.
void UiWidgets::pushButton(const TouchButton& btn) {
    const uint16_t bg = btn.pressed ? btn.pressedColor : btn.color;
    const uint16_t fg = btn.pressed ? COLOR_TEXT : COLOR_ON_DARK;
    const int16_t cx = btn.w / 2;
    const int16_t cy = (btn.h / 2) - 16;

    buttonSprite->fillSprite(COLOR_BG);
    buttonSprite->fillRoundRect(0, 0, btn.w, btn.h, BTN_RADIUS, bg);
    buttonSprite->drawRoundRect(0, 0, btn.w, btn.h, BTN_RADIUS, COLOR_BTN_BORDER);

    if (btn.pressed) {
        buttonSprite->drawRoundRect(3, 3, btn.w - 6, btn.h - 6, BTN_RADIUS - 2, fg);
    }

    if (btn.icon == ICON_DOWN) {
        buttonSprite->fillTriangle(cx - 24, cy - 16, cx + 24, cy - 16, cx, cy + 12, fg);
    } else if (btn.icon == ICON_UP) {
        buttonSprite->fillTriangle(cx - 24, cy + 12, cx + 24, cy + 12, cx, cy - 16, fg);
    }

    buttonSprite->loadFont(consolab24);
    buttonSprite->setTextDatum(MC_DATUM);
    buttonSprite->setTextColor(fg, bg);
    buttonSprite->drawString(btn.label, cx, btn.h - 32);
    buttonSprite->unloadFont();

    buttonSprite->pushSprite(btn.x, btn.y);
}

// Returns true when a point is inside the button bounds.
bool UiWidgets::buttonContains(const TouchButton& btn, int16_t x, int16_t y) {
    return hitTest(x, y, btn.x, btn.y, btn.w, btn.h);
}
