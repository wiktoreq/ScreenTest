#include "UiWidgets.h"
#include "consolab24.h"
#include <Arduino.h>

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

// Paints the slider track, fill, knob, and percent readout.
void UiWidgets::drawSlider(TFT_eSPI* tft, const HorizontalSlider& slider, bool drawChrome) {
    const int16_t knobCy = slider.trackY + (slider.trackH / 2);
    const int16_t bandY = knobCy - slider.knobRadius - 1;
    const int16_t bandH = (slider.knobRadius * 2) + 2;

    // Erase the previous knob/track strip so movement does not leave trails
    tft->fillRect(slider.trackX - slider.knobRadius, bandY,
                  slider.trackW + (slider.knobRadius * 2), bandH,
                  COLOR_SURFACE);

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

    int16_t knobX = slider.trackX + map(slider.value, 0, 100, 0, slider.trackW);
    knobX = constrain(knobX, slider.trackX, slider.trackX + slider.trackW);

    tft->fillCircle(knobX, knobCy, slider.knobRadius, COLOR_KNOB);
    tft->drawCircle(knobX, knobCy, slider.knobRadius, slider.fillColor);
    tft->fillCircle(knobX, knobCy, 6, slider.fillColor);

    char valBuf[8];
    snprintf(valBuf, sizeof(valBuf), "%3d%%", slider.value);

    tft->setTextDatum(TR_DATUM);
    tft->setTextColor(COLOR_TEXT, COLOR_SURFACE);
    tft->setTextPadding(56);
    tft->drawString(valBuf, slider.cardX + slider.cardW - 14, slider.cardY + 14, 4);
    tft->setTextPadding(0);

    if (drawChrome && slider.label != nullptr) {
        tft->loadFont(consolab24);
        tft->setTextDatum(TL_DATUM);
        tft->setTextColor(COLOR_TEXT, COLOR_SURFACE);
        tft->drawString(slider.label, slider.labelX, slider.cardY + 12);
        tft->unloadFont();
    }
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

// Paints a large rounded hold-button with optional chevron icon.
void UiWidgets::drawButton(TFT_eSPI* tft, const TouchButton& btn) {
    const uint16_t bg = btn.pressed ? btn.pressedColor : btn.color;
    const uint16_t fg = btn.pressed ? COLOR_BG : COLOR_TEXT;

    tft->fillRoundRect(btn.x, btn.y, btn.w, btn.h, BTN_RADIUS, bg);
    tft->drawRoundRect(btn.x, btn.y, btn.w, btn.h, BTN_RADIUS, COLOR_BTN_BORDER);

    if (btn.pressed) {
        tft->drawRoundRect(btn.x + 3, btn.y + 3, btn.w - 6, btn.h - 6, BTN_RADIUS - 2, fg);
    }

    const int16_t cx = btn.x + (btn.w / 2);
    const int16_t cy = btn.y + (btn.h / 2) - 16;

    if (btn.icon == ICON_DOWN) {
        tft->fillTriangle(cx - 24, cy - 16, cx + 24, cy - 16, cx, cy + 12, fg);
    } else if (btn.icon == ICON_UP) {
        tft->fillTriangle(cx - 24, cy + 12, cx + 24, cy + 12, cx, cy - 16, fg);
    }

    tft->loadFont(consolab24);
    tft->setTextDatum(MC_DATUM);
    tft->setTextColor(fg, bg);
    tft->drawString(btn.label, cx, btn.y + btn.h - 32);
    tft->unloadFont();
}

// Returns true when a point is inside the button bounds.
bool UiWidgets::buttonContains(const TouchButton& btn, int16_t x, int16_t y) {
    return hitTest(x, y, btn.x, btn.y, btn.w, btn.h);
}
