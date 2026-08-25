#include <Arduino.h>
#include <TFT_eSPI.h>

#include "Config.h"
#include "TopMenu.h"
#include "MainMenu.h"

#include <SPI.h>
#include "FS.h"
#include "SPIFFS.h"
#define CALIBRATION_FILE "/TouchCalData"
#define REPEAT_CAL false

#include "consola28.h"
#include "consolab28.h"

TFT_eSPI tft = TFT_eSPI();
TopMenu topMenu(&tft);
// 1. Declare the Sprite object, passing a pointer to the main TFT instance
TFT_eSprite contentSprite = TFT_eSprite(&tft);
MainMenu mainMenu(&tft);

enum AppState
{
  STATE_MAIN = 0,
  STATE_HEIGHT_ACTUATOR = 1,
  STATE_POS_ACTUATOR = 2
};

AppState currentState = STATE_MAIN;

// Hardware Actuator Function
void updateActuatorHardware(uint8_t targetPosition, uint8_t movementSpeed);

void touch_calibrate();

void drawContentArea();

void setup()
{
  Serial.begin(115200);
  delay(500);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  if (REPEAT_CAL)
  {
    touch_calibrate();
    tft.fillScreen(TFT_BLACK);
  }

  // -- SPRITE SETUP --
  // Configure sprite color depth (16-bit is standard for ILI9341/ST7789)
  contentSprite.setColorDepth(16);

  // Allocate memory for the Sprite.
  // This creates a buffer of TFT_W x CONTENT_H pixels.
  if (!contentSprite.createSprite(TFT_W, CONTENT_H))
  {
    Serial.println("Error: Not enough RAM to create Sprite!");
    while (1)
      delay(100); // Halt if memory allocation fails
  }

  // Setup Top Menu
  topMenu.addTab("Main");
  topMenu.addTab("Height");
  topMenu.addTab("Desc position");

  topMenu.init();
  topMenu.draw();

  mainMenu.init();
  mainMenu.draw();
  delay(150);

  // Draw initial content
  drawContentArea();
}

void loop()
{
  uint16_t touchX = 0, touchY = 0;

  if (tft.getTouch(&touchX, &touchY))
  {

    if (touchY <= MENU_HEIGHT)
    {
      int newTab = topMenu.handleTouch(touchX, touchY);
      Serial.printf("%d", newTab);

      if (newTab != -1)
      {
        currentState = static_cast<AppState>(newTab);

        if (currentState == STATE_MAIN)
        {
          mainMenu.draw();
        }
        delay(150);
      }
    }
    else
    {
      // Touch in Content Zone
      if (currentState == STATE_MAIN)
      {
        // Route touch to screen logic
        mainMenu.handleTouch(touchX, touchY);

        // Read position and speed values directly from screen instance
        uint8_t currentPos = mainMenu.getPosition();
        uint8_t currentSpeed = mainMenu.getSpeed();

        // Command actuator hardware
        updateActuatorHardware(currentPos, currentSpeed);
      }
    }
  }
}

void drawContentArea()
{
  // 2. Clear the SPRITE (not the TFT). This happens entirely in RAM.
  contentSprite.fillSprite(COLOR_BG);

  contentSprite.loadFont(consola28);
  // 3. Draw directly to the Sprite instead of the TFT
  contentSprite.setTextDatum(MC_DATUM);
  contentSprite.setTextColor(TFT_WHITE, COLOR_BG);

  // Note: The Y coordinate is now relative to the SPRITE, not the TFT!
  // So the middle of the sprite is (CONTENT_H / 2), not (CONTENT_Y + CONTENT_H/2)
  int spriteMiddleY = CONTENT_H / 2;
  int spriteMiddleX = TFT_W / 2;

  switch (currentState)
  {
  case STATE_MAIN:
    mainMenu.draw();
    break;

  case STATE_HEIGHT_ACTUATOR:
    contentSprite.drawString("Settings Loaded", spriteMiddleX, spriteMiddleY);
    break;

  case STATE_POS_ACTUATOR:
    contentSprite.drawString("CPU: 42% | RAM: 18%", spriteMiddleX, spriteMiddleY);
    break;
  }

  // 4. Push the completely drawn Sprite to the physical display
  // The arguments are the X and Y coordinates on the TFT where the Sprite should appear.
  contentSprite.unloadFont();
  contentSprite.pushSprite(0, CONTENT_Y);
}

// Hardware Actuator Function
void updateActuatorHardware(uint8_t targetPosition, uint8_t movementSpeed)
{
  // Pass targetPosition (0-100%) and movementSpeed (0-100%) to your motor/PWM driver
  // Example: motorDriver.setTarget(targetPosition, movementSpeed);
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin())
  {
    Serial.println("formatting file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE))
  {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      fs::File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f)
      {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL)
  {
    // calibration data valid
    tft.setTouch(calData);
  }
  else
  {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL)
    {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    fs::File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f)
    {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}