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
}

void loop(){ 
  jg.show_accelerometer(10, 40);
  tft.fillRect(49, 75, 40, 22, jg.background.color_16());
  jg.draw_string(10, 75, "Joy X:", jg.stroke.color_16(), 1);
  jg.print_integer(50, 75, analogRead(0), 1);
  jg.draw_string(10, 85, "Joy Y:", jg.stroke.color_16(), 1);
  jg.print_integer(50, 85, analogRead(1), 1);
}
