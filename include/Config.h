#pragma once
#include <stdint.h>

// Physical display size in landscape (ILI9488 rotation 3)
constexpr int16_t TFT_W = 480;
constexpr int16_t TFT_H = 320;

// Top tab bar
constexpr int16_t MENU_HEIGHT = 44;
constexpr int16_t CONTENT_Y   = MENU_HEIGHT + 1;
constexpr int16_t CONTENT_H   = TFT_H - CONTENT_Y;

// Shared spacing and control sizes (touch-friendly on a 480x320 panel)
constexpr int16_t PAD              = 10;
constexpr int16_t GAP              = 10;
constexpr int16_t CARD_RADIUS      = 12;
constexpr int16_t BTN_RADIUS       = 12;
constexpr int16_t SLIDER_TRACK_H   = 14;
constexpr int16_t SLIDER_KNOB_R    = 16;
constexpr int16_t TOUCH_HYSTERESIS = 3;

// Main menu: two equal brightness cards stacked in the content area
constexpr int16_t MAIN_CARD_H = (CONTENT_H - PAD * 2 - GAP) / 2;

// Actuator menus: speed card on top, two large hold-buttons below
constexpr int16_t ACT_SLIDER_CARD_H = 96;
constexpr int16_t ACT_BTN_Y         = CONTENT_Y + PAD + ACT_SLIDER_CARD_H + GAP;
constexpr int16_t ACT_BTN_H         = (CONTENT_Y + CONTENT_H) - PAD - ACT_BTN_Y;
constexpr int16_t ACT_BTN_W         = (TFT_W - PAD * 2 - GAP) / 2;

// Palette (RGB565). Hex comments are the intended 24-bit colors.
constexpr uint16_t COLOR_BG           = 0x10C4; // #121820
constexpr uint16_t COLOR_SURFACE      = 0x1947; // #1E2838
constexpr uint16_t COLOR_TAB_IDLE     = 0x31EA; // #323C50
constexpr uint16_t COLOR_TAB_ACTIVE   = 0x0C4F; // #0A8A7A
constexpr uint16_t COLOR_ACCENT       = 0x2719; // #20E0C8
constexpr uint16_t COLOR_TEXT         = 0xFFFF; // #FFFFFF
constexpr uint16_t COLOR_TEXT_DIM     = 0x9D36; // #9AA4B0
constexpr uint16_t COLOR_TRACK        = 0x3A2B; // #3A4658
constexpr uint16_t COLOR_KNOB         = 0xFFFF; // #FFFFFF
constexpr uint16_t COLOR_SEPARATOR    = 0x10C4; // #121820

constexpr uint16_t COLOR_SCREEN_FILL  = 0xFD84; // #FFB020
constexpr uint16_t COLOR_LIGHT_FILL   = 0x45BF; // #40B0FF

constexpr uint16_t COLOR_BTN_CONTRACT         = 0xD305; // #D06028
constexpr uint16_t COLOR_BTN_CONTRACT_PRESSED = 0xFE90; // #FFD080
constexpr uint16_t COLOR_BTN_RETRACT          = 0x23D9; // #2078C8
constexpr uint16_t COLOR_BTN_RETRACT_PRESSED  = 0x96BF; // #90D0FF
constexpr uint16_t COLOR_BTN_BORDER           = 0x8CB4; // #8A94A0
