/* LucidTronix Joy Gamer SD Navigator
 * If the compiler gives you an error about sketch being too big make sure to
 * COMMENT OUT both the USE_ACCELEROMETER and the USE COLOR defines in the JoyGamer.h file.  
 * They should look like this:
 //#define USE_ACCELEROMETER
 //#define USE_COLOR
 * For more instructions, details and schematic, See:
 * http://www.lucidtronix.com/tutorials/21
 */

#include <JoyGamer.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

Adafruit_ST7735  tft = Adafruit_ST7735(SS, 9, 8);
JoyGamer jg = JoyGamer(&tft);

enum modes_t{
  DISPLAY_DIRECTORY,
  DISPLAY_BITMAP,
  DISPLAY_TEXT
};

modes_t mode = DISPLAY_DIRECTORY;

int cursor_x = 64;
int cursor_y = 64;

File cur_file;
File selected_file;
int cur_dir_files;
const int max_files = 12;
String files[max_files];

int line_height = 12;
int char_width = 6;
unsigned long last_switch = 0;

void setup(){
  jg.initialize();
  jg.start_sd_card();
  open_file("/");
}

void loop(){
  switch(mode){
    case DISPLAY_DIRECTORY:
      handle_cursor();
      for(int i = 0; i < cur_dir_files; i++){
        if(cursor_y > i*10+ 10 && cursor_y < i*10 + 20){
          jg.draw_string(10, i*10 + 10, files[i], ST7735_MAGENTA);
          if(digitalRead(jg.joystick_btn_pin) == HIGH) {
            open_file(files[i]);
            break;
          }
        } 
        else {
          jg.draw_string(10, i*10 + 10, files[i], ST7735_BLACK);
        }
        if(digitalRead(jg.btn_pin) == LOW && millis() - last_switch > 500) {
          open_file("/");
          break;
        }
      }
      break;
    case DISPLAY_TEXT:
      jg.clear_screen(ST7735_WHITE);
      display_text_file();
      break;
    case DISPLAY_BITMAP:
      jg.bmp_draw(cur_file);
      delay(2000);
      open_file("/");
      break;
  }
}

void handle_cursor(){
  tft.fillCircle(cursor_x, cursor_y, 5, ST7735_WHITE);
  int delta_x = map(analogRead(0), 0, 1024, -6, 6); 
  int delta_y = map(analogRead(1), 0, 1024, -8, 8);
  cursor_x = constrain(cursor_x + delta_x, 5, 123);
  cursor_y = constrain(cursor_y + delta_y, 5, 155); 
  tft.fillCircle(cursor_x, cursor_y, 5, ST7735_GREEN);
}

void open_file(String filename){
  jg.clear_screen(ST7735_WHITE);
  cur_file.seek(0);
  cur_file.close();
  delay(100);
  char buffer[13];
  filename.toCharArray(buffer, 13);
  cur_file = SD.open(buffer);
  filename.toLowerCase();
  if(cur_file.isDirectory()){
    mode = DISPLAY_DIRECTORY;
    load_dir(cur_file);
  }
  else if(filename.lastIndexOf(".bmp") != -1){
    mode = DISPLAY_BITMAP;
  }
  else {
    mode = DISPLAY_TEXT;
  }
  last_switch = millis();
}

void load_dir(File dir){
  for(int i = 0; i < max_files; i++) files[i] = "";
  jg.clear_screen(ST7735_WHITE);
  dir.rewindDirectory();
  File entry =  dir.openNextFile();
  cur_dir_files = 0;
  while(entry && SD.exists(entry.name()) && max_files > cur_dir_files){
    String filename = entry.name();
    if(filename[0] != '.' && filename[0] != '_' && filename.lastIndexOf('~') == -1){ // don't show hidden files
      files[cur_dir_files] = filename;
      cur_dir_files++;
    }
    entry.close();
    entry = dir.openNextFile();
  }
  dir.rewindDirectory();
}

void display_text_file(){
  int aax = 2;
  int aay = 3;
  tft.setTextColor(ST7735_BLACK);
  while(cur_file && cur_file.available() && aay < 150){
    char cur_c = cur_file.read();
    if(cur_c == 10 || cur_c == 13 || (cur_c == ' ' && aax > 65)){
      aay += line_height;
      aax = 2;
    }
    else {   
      tft.setCursor(aax, aay);
      tft.print(cur_c);
      aax += char_width;
      if (aax >= 118) {
        aax = 2;
        aay += line_height;
      }
    }
    delay(10);
    if(digitalRead(jg.btn_pin) == LOW && millis() - last_switch > 500) {
      open_file("/");
      break;
    }
  }
  delay(2500);
}

