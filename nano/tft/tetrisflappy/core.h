#include <SPI.h>
#include "src/Adafruit_GFX/Adafruit_GFX.h"
#include "src/Adafruit_ST7735/Adafruit_ST7735.h"

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
// #define SD_CS    4  // Chip select line for SD card
#define TFT_CS            10  // Chip select line for TFT display
#define TFT_DC            9   // Data/command line for TFT
#define TFT_RST           8   // Reset line for TFT (or connect to +5V)
#define PUSH_BUTTON       2   // button

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);