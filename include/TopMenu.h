#pragma once
#include <TFT_eSPI.h>
#include <vector>
#include <string>

class TopMenu {
private:
    TFT_eSPI* tft;
    std::vector<std::string> tabNames;
    int activeTabIndex;
    int tabWidth;

public:
    // Stores the display driver used to paint the tab bar.
    TopMenu(TFT_eSPI* tftInstance);

    // Registers a tab label; call before init().
    void addTab(const std::string& name);

    // Computes equal tab widths from the current label list.
    void init();

    // Redraws the full top tab bar and active-tab highlight.
    void draw();

    // Selects a tab from a touch in the bar; returns the new index, or -1 if unchanged.
    int handleTouch(int16_t x, int16_t y);

    // Returns the currently selected tab index.
    int getActiveTab() const { return activeTabIndex; }
};
