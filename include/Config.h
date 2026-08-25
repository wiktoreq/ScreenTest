#pragma once
#include <stdint.h>

// Screen Dimensions
constexpr int16_t TFT_W = 480; // Adjust for your rotation
constexpr int16_t TFT_H = 320;

// Top Menu Layout
constexpr int16_t MENU_HEIGHT = 40;
constexpr int16_t CONTENT_Y   = MENU_HEIGHT + 1;
constexpr int16_t CONTENT_H   = TFT_H - MENU_HEIGHT;

//Main Menu layout
constexpr int16_t SLIDER_HEIGHT = 240;
constexpr int16_t SLIDER_WIDTH  = 5;
constexpr int16_t KNOB_RADIUS   = 5;



// Colors
constexpr uint32_t COLOR_BG        = 0x0000; // TFT_BLACK
constexpr uint32_t COLOR_MENU_BG   = 0x2104; // Dark Grey
constexpr uint32_t COLOR_TAB_IDLE  = 0x4208; // Lighter Grey
constexpr uint32_t COLOR_TAB_ACTIVE= 0x03E0; // TFT_DARKGREEN
constexpr uint32_t COLOR_TEXT      = 0xFFFF; // TFT_WHITE