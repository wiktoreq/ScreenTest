#include "MainMenu.h"
#include <Arduino.h>

MainMenu::MainMenu(TFT_eSPI* tftInstance) 
    : tft(tftInstance), knobSprite(tftInstance) {}

MainMenu::~MainMenu() {
    knobSprite.deleteSprite(); // Free memory when screen is destroyed
}

void MainMenu::init() {
    // Pos Slider Setup
    posSlider.x = 80;
    posSlider.yMin = 40 + CONTENT_Y;  // We now use absolute TFT Y coordinates
    posSlider.yMax = 190 + CONTENT_Y; 
    posSlider.radius = 16;
    posSlider.value = 50;
    posSlider.knobY = posSlider.yMax - ((posSlider.yMax - posSlider.yMin) / 2);
    posSlider.oldKnobY = posSlider.knobY;
    posSlider.label = "POSITION";

    // Speed Slider Setup
    speedSlider.x = 240;
    speedSlider.yMin = 40 + CONTENT_Y;
    speedSlider.yMax = 190 + CONTENT_Y;
    speedSlider.radius = 16;
    speedSlider.value = 20;
    speedSlider.knobY = speedSlider.yMax - map(speedSlider.value, 0, 100, 0, (speedSlider.yMax - speedSlider.yMin));
    speedSlider.oldKnobY = speedSlider.knobY;
    speedSlider.label = "SPEED";

    // Allocate a tiny 34x34 sprite (Radius 16 * 2 + 2px padding)
    knobSprite.setColorDepth(16);
    knobSprite.createSprite(34, 34);
    renderKnobSprite();
}

// Pre-renders the knob into RAM. This is called ONCE.
void MainMenu::renderKnobSprite() {
    knobSprite.fillSprite(COLOR_BG);
    
    // Draw the track segment INSIDE the sprite so it seamlessly blends with the background
    knobSprite.drawLine(17, 0, 17, 34, TFT_DARKGREY);
    knobSprite.drawLine(16, 0, 16, 34, TFT_WHITE);
    
    // Draw the actual knob
    knobSprite.fillCircle(17, 17, 16, TFT_BLUE);
    knobSprite.drawCircle(17, 17, 16, TFT_WHITE);
    knobSprite.fillCircle(17, 17, 8, TFT_WHITE);
}

void MainMenu::drawStaticBackground() {
    // 1. Clear ONLY the content area on the physical TFT
    tft->fillRect(0, CONTENT_Y, TFT_W, CONTENT_H, COLOR_BG);

    // 2. Draw static tracks
    tft->drawLine(posSlider.x, posSlider.yMin, posSlider.x, posSlider.yMax, TFT_DARKGREY);
    tft->drawLine(posSlider.x - 1, posSlider.yMin, posSlider.x - 1, posSlider.yMax, TFT_WHITE);
    
    tft->drawLine(speedSlider.x, speedSlider.yMin, speedSlider.x, speedSlider.yMax, TFT_DARKGREY);
    tft->drawLine(speedSlider.x - 1, speedSlider.yMin, speedSlider.x - 1, speedSlider.yMax, TFT_WHITE);

    // 3. Draw static labels
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(TFT_CYAN, COLOR_BG);
    tft->drawString(posSlider.label, posSlider.x, CONTENT_Y + 10, 2);
    tft->drawString(speedSlider.label, speedSlider.x, CONTENT_Y + 10, 2);
}

void MainMenu::updateSliderValueText(const VerticalSlider& slider) {
    char valBuf[8];
    snprintf(valBuf, sizeof(valBuf), "%d%%", slider.value);
    
    tft->setTextDatum(TC_DATUM);
    tft->setTextColor(TFT_WHITE, COLOR_BG);
    
    // PRO TIP: setTextPadding automatically draws a background box of this width 
    // before drawing the text. This prevents characters from overlapping without flickering.
    tft->setTextPadding(40); 
    tft->drawString(valBuf, slider.x, slider.yMax + 20, 2);
    tft->setTextPadding(0); // Reset padding
}

void MainMenu::updateSliderVisuals(VerticalSlider& slider) {
    if (slider.knobY == slider.oldKnobY) return; // No movement, do nothing

    // 1. ERASE trail: Fill a black box over the OLD knob position
    tft->fillRect(slider.x - 17, slider.oldKnobY - 17, 34, 34, COLOR_BG);

    // 2. RESTORE track: Redraw the missing segment of the track in that black box
    tft->drawLine(slider.x, slider.oldKnobY - 17, slider.x, slider.oldKnobY + 17, TFT_DARKGREY);
    tft->drawLine(slider.x - 1, slider.oldKnobY - 17, slider.x - 1, slider.oldKnobY + 17, TFT_WHITE);

    // 3. DRAW new knob: Push the pre-rendered knob sprite to the NEW position
    knobSprite.pushSprite(slider.x - 17, slider.knobY - 17);

    // 4. Update the text readout
    updateSliderValueText(slider);

    slider.oldKnobY = slider.knobY;
}

void MainMenu::draw() {
    drawStaticBackground();
    
    // Force an initial draw of the knobs and text
    knobSprite.pushSprite(posSlider.x - 17, posSlider.knobY - 17);
    updateSliderValueText(posSlider);
    
    knobSprite.pushSprite(speedSlider.x - 17, speedSlider.knobY - 17);
    updateSliderValueText(speedSlider);
}

void MainMenu::handleTouch(int16_t touchX, int16_t touchY) {
    constexpr int16_t TOUCH_MARGIN_X = 40; // Wide X margin for fat fingers
    
    VerticalSlider* activeSlider = nullptr;

    if (touchX >= (posSlider.x - TOUCH_MARGIN_X) && touchX <= (posSlider.x + TOUCH_MARGIN_X)) {
        activeSlider = &posSlider;
    } 
    else if (touchX >= (speedSlider.x - TOUCH_MARGIN_X) && touchX <= (speedSlider.x + TOUCH_MARGIN_X)) {
        activeSlider = &speedSlider;
    }

    if (activeSlider != nullptr) {
        int16_t constrainedY = constrain(touchY, activeSlider->yMin, activeSlider->yMax);
        
        // HYSTERESIS: Ignore touch noise. Only update if the finger moved more than 2 pixels.
        if (abs(constrainedY - activeSlider->knobY) > 2) {
            activeSlider->knobY = constrainedY;
            activeSlider->value = map(activeSlider->knobY, activeSlider->yMax, activeSlider->yMin, 0, 100);
            
            updateSliderVisuals(*activeSlider);
        }
    }
}