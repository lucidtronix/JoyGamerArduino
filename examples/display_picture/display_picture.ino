/* LucidTronix Joy Gamer
 * For instructions, details and schematic, See:
 * http://www.lucidtronix.com/tutorials/48
 */

#include <JoyGamer.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Color.h>
#include <I2C.h>
#include <MMA8453Q.h>
#include <SD.h>

Adafruit_ST7735  tft = Adafruit_ST7735(SS, 9, 8);
JoyGamer jg = JoyGamer(&tft);

void setup(){
  jg.initialize();
  jg.start_sd_card();
  jg.bmp_load_and_draw_image("sample.bmp");
}

void loop() {

}


