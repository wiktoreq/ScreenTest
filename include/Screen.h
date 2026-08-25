#pragma once
#include <stdint.h>

// Common interface for every content-area screen (Main, Height, Position).
class Screen {
public:
    // Releases any subclass resources.
    virtual ~Screen() {}

    // Prepares layout, default values, and sprites before the first draw.
    virtual void init() = 0;

    // Paints the full content area for this screen.
    virtual void draw() = 0;

    // Routes a content-area touch to sliders or buttons.
    virtual void handleTouch(int16_t x, int16_t y) = 0;

    // Clears pressed/drag state when the finger lifts.
    virtual void handleRelease() {}
};
