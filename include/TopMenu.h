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
    TopMenu(TFT_eSPI* tftInstance);
    
    void addTab(const std::string& name);
    void init();
    void draw();
    
    // Returns the new tab index if a tab was tapped, otherwise -1
    int handleTouch(int16_t x, int16_t y);
    
    int getActiveTab() const { return activeTabIndex; }
};