#include "TopMenu.h"
#include "Config.h"
#include "consola28.h"

// Stores the display driver used to paint the tab bar.
TopMenu::TopMenu(TFT_eSPI* tftInstance) : tft(tftInstance), activeTabIndex(0), tabWidth(0) {}

// Registers a tab label; call before init().
void TopMenu::addTab(const std::string& name) {
    tabNames.push_back(name);
}

// Computes equal tab widths from the current label list.
void TopMenu::init() {
    if (tabNames.empty()) return;
    tabWidth = TFT_W / tabNames.size();
}

// Redraws the full top tab bar and active-tab highlight.
void TopMenu::draw() {
    tft->loadFont(consola28);
    tft->setTextDatum(MC_DATUM);

    for (size_t i = 0; i < tabNames.size(); i++) {
        const int16_t xPos = i * tabWidth;
        const uint16_t tabColor = (i == (size_t)activeTabIndex) ? COLOR_TAB_ACTIVE : COLOR_TAB_IDLE;

        tft->fillRect(xPos, 0, tabWidth, MENU_HEIGHT, tabColor);

        if (i > 0) {
            tft->drawFastVLine(xPos, 8, MENU_HEIGHT - 16, COLOR_TRACK);
        }

        const uint16_t labelColor = (i == (size_t)activeTabIndex) ? COLOR_TEXT : COLOR_TEXT_DIM;
        tft->setTextColor(labelColor, tabColor);
        tft->drawString(tabNames[i].c_str(), xPos + (tabWidth / 2), (MENU_HEIGHT / 2) - 2);
    }

    tft->drawFastHLine(0, MENU_HEIGHT, TFT_W, COLOR_ACCENT);

    // Active-tab underline so the selected category is obvious at a glance
    const int16_t activeX = activeTabIndex * tabWidth;
    tft->fillRect(activeX + 12, MENU_HEIGHT - 3, tabWidth - 24, 3, COLOR_ACCENT);

    tft->unloadFont();
}

// Selects a tab from a touch in the bar; returns the new index, or -1 if unchanged.
int TopMenu::handleTouch(int16_t x, int16_t y) {
    if (y > MENU_HEIGHT || tabWidth <= 0) return -1;

    int tappedIndex = x / tabWidth;
    if (tappedIndex < 0) tappedIndex = 0;
    if (tappedIndex >= (int)tabNames.size()) tappedIndex = (int)tabNames.size() - 1;

    if (tappedIndex != activeTabIndex) {
        activeTabIndex = tappedIndex;
        draw();
        return activeTabIndex;
    }
    return -1;
}
