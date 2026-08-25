#include "TopMenu.h"
#include "Config.h"

// 1. Include your generated font array here
#include "consola28.h"

TopMenu::TopMenu(TFT_eSPI* tftInstance) : tft(tftInstance), activeTabIndex(0), tabWidth(0) {}

void TopMenu::addTab(const std::string& name) {
    tabNames.push_back(name);
}

void TopMenu::init() {
    if (tabNames.empty()) return;
    tabWidth = TFT_W / tabNames.size();
}

void TopMenu::draw() {
    // 2. Load the font from PROGMEM into SRAM before drawing
    tft->loadFont(consola28);
    
    tft->setTextDatum(MC_DATUM);
    
    for (size_t i = 0; i < tabNames.size(); i++) {
        int16_t xPos = i * tabWidth;
        
        // Determine if this specific tab is active or idle
        uint32_t tabColor = (i == activeTabIndex) ? COLOR_TAB_ACTIVE : COLOR_TAB_IDLE;
        
        // Draw the physical tab background rectangle
        tft->fillRect(xPos, 0, tabWidth - 1, MENU_HEIGHT, tabColor);
        
        // 3. CRITICAL: Pass 'tabColor' as the background argument.
        // The engine uses this to calculate the alpha-blending gradient for the font edges.
        tft->setTextColor(COLOR_TEXT, tabColor);
        
        // 4. Draw the string. 
        // Notice we removed the ', 2' at the end that was used for the default GLCD font.
        tft->drawString(tabNames[i].c_str(), xPos + (tabWidth / 2), MENU_HEIGHT / 2); 
    }
    
    // Draw the bottom separator line
    tft->drawFastHLine(0, MENU_HEIGHT, TFT_W, TFT_WHITE);
    
    // 5. Unload the font immediately to free up internal RAM for your content screens
    tft->unloadFont();
}

int TopMenu::handleTouch(int16_t x, int16_t y) {
    if (y > MENU_HEIGHT) return -1;
    
    int tappedIndex = x / tabWidth;
    
    if (tappedIndex >= 0 && tappedIndex < tabNames.size()) {
        if (tappedIndex != activeTabIndex) {
            activeTabIndex = tappedIndex;
            draw(); 
            return activeTabIndex;
        }
    }
    return -1; 
}