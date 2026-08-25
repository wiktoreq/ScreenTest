#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "FS.h"
#include "SPIFFS.h"

#include "Config.h"
#include "TopMenu.h"
#include "MainMenu.h"
#include "ActuatorMenu.h"

#define CALIBRATION_FILE "/TouchCalData"
#define REPEAT_CAL false

TFT_eSPI tft = TFT_eSPI();
TopMenu topMenu(&tft);
MainMenu mainMenu(&tft);
ActuatorMenu heightMenu(&tft);
ActuatorMenu positionMenu(&tft);

enum AppState {
    STATE_MAIN = 0,
    STATE_HEIGHT = 1,
    STATE_POSITION = 2
};

AppState currentState = STATE_MAIN;
Screen* currentScreen = &mainMenu;
bool touchWasActive = false;

// Maps a tab index to its content screen and draws that screen.
void switchToState(AppState newState);

// Placeholder for later hardware work: screen and external-light brightness.
void updateBrightnessHardware(uint8_t screenBrightness, uint8_t lightBrightness);

// Placeholder for later hardware work: actuator speed plus hold-buttons.
void updateActuatorHardware(uint8_t movementSpeed, bool contracting, bool retracting);

// Loads stored touch calibration or runs the on-screen calibration wizard.
void touch_calibrate();

// Maps a tab index to its content screen and draws that screen.
void switchToState(AppState newState) {
    currentState = newState;
    switch (currentState) {
        case STATE_HEIGHT:
            currentScreen = &heightMenu;
            break;
        case STATE_POSITION:
            currentScreen = &positionMenu;
            break;
        case STATE_MAIN:
        default:
            currentScreen = &mainMenu;
            break;
    }
    currentScreen->draw();
}

// Placeholder for later hardware work: screen and external-light brightness.
void updateBrightnessHardware(uint8_t screenBrightness, uint8_t lightBrightness) {
    (void)screenBrightness;
    (void)lightBrightness;
}

// Placeholder for later hardware work: actuator speed plus hold-buttons.
void updateActuatorHardware(uint8_t movementSpeed, bool contracting, bool retracting) {
    (void)movementSpeed;
    (void)contracting;
    (void)retracting;
}

// Loads stored touch calibration or runs the on-screen calibration wizard.
void touch_calibrate() {
    uint16_t calData[5];
    uint8_t calDataOK = 0;

    if (!SPIFFS.begin()) {
        Serial.println("formatting file system");
        SPIFFS.format();
        SPIFFS.begin();
    }

    if (SPIFFS.exists(CALIBRATION_FILE)) {
        if (REPEAT_CAL) {
            SPIFFS.remove(CALIBRATION_FILE);
        } else {
            fs::File f = SPIFFS.open(CALIBRATION_FILE, "r");
            if (f) {
                if (f.readBytes((char*)calData, 14) == 14) {
                    calDataOK = 1;
                }
                f.close();
            }
        }
    }

    if (calDataOK && !REPEAT_CAL) {
        tft.setTouch(calData);
        return;
    }

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.println("Touch corners as indicated");
    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    fs::File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
        f.write((const unsigned char*)calData, 14);
        f.close();
    }
}

// Initializes the display, tab bar, and all three content screens.
void setup() {
    Serial.begin(115200);
    delay(500);

    tft.init();
    tft.setRotation(3);
    tft.fillScreen(COLOR_BG);

    if (REPEAT_CAL) {
        touch_calibrate();
        tft.fillScreen(COLOR_BG);
    }

    topMenu.addTab("Main");
    topMenu.addTab("Height");
    topMenu.addTab("Position");
    topMenu.init();
    topMenu.draw();

    mainMenu.init();
    heightMenu.init();
    positionMenu.init();

    switchToState(STATE_MAIN);
}

// Polls the touchscreen and routes taps to the tab bar or the active content screen.
void loop() {
    uint16_t touchX = 0;
    uint16_t touchY = 0;
    const bool touched = tft.getTouch(&touchX, &touchY);

    if (touched) {
        if (touchY <= MENU_HEIGHT) {
            const int newTab = topMenu.handleTouch(touchX, touchY);
            if (newTab != -1) {
                currentScreen->handleRelease();
                switchToState(static_cast<AppState>(newTab));
            }
        } else {
            currentScreen->handleTouch(touchX, touchY);

            if (currentState == STATE_MAIN) {
                updateBrightnessHardware(mainMenu.getScreenBrightness(),
                                         mainMenu.getLightBrightness());
            } else if (currentState == STATE_HEIGHT) {
                updateActuatorHardware(heightMenu.getSpeed(),
                                       heightMenu.isContractPressed(),
                                       heightMenu.isRetractPressed());
            } else {
                updateActuatorHardware(positionMenu.getSpeed(),
                                       positionMenu.isContractPressed(),
                                       positionMenu.isRetractPressed());
            }
        }
        touchWasActive = true;
    } else if (touchWasActive) {
        currentScreen->handleRelease();
        touchWasActive = false;
    }
}
