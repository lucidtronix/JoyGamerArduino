/* LucidTronix Joy Gamer SD Navigator
 * If the compiler gives you an error about sketch being too big make sure to
 * COMMENT OUT both the USE_ACCELEROMETER and the USE COLOR defines in the JoyGamer.h file.  
 * They should look like this:
 //#define USE_ACCELEROMETER
 //#define USE_COLOR
 * For more instructions, details and schematic, See:
 * http://www.lucidtronix.com/tutorials/65
 */

#include <JoyGamer.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Color.h>

#include <SD.h>

// for leo:
Adafruit_ST7735  tft = Adafruit_ST7735(SS, 9, 8);
//for uno etc.. 
//Adafruit_ST7735 tft = ST7735(10, 9, 8);
// for mega
//Adafruit_ST7735 tft = ST7735(53, 9, 8);

JoyGamer jg = JoyGamer(&tft);

int cur_pic = 0;
const int num_pics = 29;
String base_name = "face";
String pic_name = base_name + "02.bmp";
void setup(){
  jg.initialize();
  jg.clear_screen();
  jg.start_sd_card();
}

void loop() {
 cur_pic = random(0, num_pics);
 if(cur_pic < 10) pic_name = base_name + String("0") + cur_pic + String(".bmp");
 else if(cur_pic < 100) pic_name = base_name + cur_pic + String(".bmp");
 jg.bmp_load_and_draw_image(pic_name);
 delay(3000);
}


