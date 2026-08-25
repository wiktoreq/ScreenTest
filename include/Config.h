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

// Object-sized sprites: each buffer is only as large as the widget it paints
constexpr int16_t KNOB_SPRITE     = (SLIDER_KNOB_R * 2) + 2;
constexpr uint16_t KNOB_CHROMA    = 0xF81F; // magenta key, unused in the UI
constexpr int16_t VALUE_SPRITE_W  = 56;     // fits "100%" in font 4
constexpr int16_t VALUE_SPRITE_H  = 26;

// Palette (RGB565) — dark, restrained scheme from the product colors.
constexpr uint16_t COLOR_BG           = 0x0084; // #041322
constexpr uint16_t COLOR_SURFACE      = 0x1926; // #1B2431
constexpr uint16_t COLOR_TAB_IDLE     = 0x0084; // #041322
constexpr uint16_t COLOR_TAB_ACTIVE   = 0x1926; // #1B2431
constexpr uint16_t COLOR_ACCENT       = 0xFFDE; // #F8F6F1
constexpr uint16_t COLOR_TEXT         = 0xFFDE; // #F8F6F1
constexpr uint16_t COLOR_TEXT_DIM     = 0xAD76; // #A9ACB6
constexpr uint16_t COLOR_TRACK        = 0x636F; // #646D7E
constexpr uint16_t COLOR_KNOB         = 0xFFFF; // #FFFFFF
constexpr uint16_t COLOR_SEPARATOR    = 0x0084; // #041322

constexpr uint16_t COLOR_SCREEN_FILL  = 0xFFDE; // #F8F6F1
constexpr uint16_t COLOR_LIGHT_FILL   = 0xAD76; // #A9ACB6

constexpr uint16_t COLOR_BTN_CONTRACT         = 0x1926; // #1B2431
constexpr uint16_t COLOR_BTN_CONTRACT_PRESSED = 0xFFDE; // #F8F6F1
constexpr uint16_t COLOR_BTN_RETRACT          = 0x636F; // #646D7E
constexpr uint16_t COLOR_BTN_RETRACT_PRESSED  = 0xFFDE; // #F8F6F1
constexpr uint16_t COLOR_BTN_BORDER           = 0xAD76; // #A9ACB6
