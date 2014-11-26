/* LucidTronix Joy Gamer SD Navigator
 * If the compiler gives you an error about sketch being too big make sure to
 * COMMENT OUT both the USE_ACCELEROMETER and the USE COLOR defines in the JoyGamer.h file.  
 * They should look like this:
 //#define USE_ACCELEROMETER
 //#define USE_COLOR
 * For more instructions, details and schematic, See:
 * http://www.lucidtronix.com/tutorials/27
 */

#include <JoyGamer.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

Adafruit_ST7735  tft = Adafruit_ST7735(SS, 9, 8);
JoyGamer jg = JoyGamer(&tft);

File bmpFile1;
File bmpFile2;

// information we extract about the bitmap file
int bmpWidth, bmpHeight;
int bmpWidth2, bmpHeight2;
uint8_t bmpDepth, bmpImageoffset;
uint8_t bmpDepth2, bmpImageoffset2;
float ratio = 0.5;

int img_index1 = 12;
int img_index2 = 16;
int num_images = 60;

void setup(void) {
  jg.initialize();
  jg.start_sd_card();
  bmp_load_and_draw_2_images("face36.bmp","face34.bmp");
}

void loop() {
  bmpdraw_2_images();
}

void bmp_load_and_draw_2_images(char* filename1, char* filename2) {
  bmpFile1 = SD.open(filename1);
  bmpFile2 = SD.open(filename2);
  if (!bmpFile1) {
    jg.draw_string(12,62, "Coodn't find image", ST7735_RED);
  }

  if (!bmpReadHeader(bmpFile1,0)) { 
    jg.draw_string(12,82, "Bad image" , ST7735_RED);
  }
  if (!bmpFile2) {
    jg.draw_string(12,62, "Coodn't find image 2" , ST7735_RED);
  }
  if (!bmpReadHeader(bmpFile2,1)) { 
    jg.draw_string(12,82, "Bad image 2" , ST7735_RED);
  }
  bmpdraw_2_images();
}

/*********************************************/
// This procedure reads 2 bitmaps and draws an average to the screen
// its sped up by reading many pixels worth of data at a time
// instead of just one pixel at a time. increading the buffer takes
// more RAM but makes the drawing a little faster. 20 pixels' worth
// is probably a good place

#define BUFFPIXEL 20
void bmpdraw_2_images() {
  bmpFile1.seek(bmpImageoffset);
  bmpFile2.seek(bmpImageoffset2);
  uint32_t time = millis();
  uint16_t p, p2; 
  uint8_t g, b;
  uint8_t g2, b2;
  int i, j;
  unsigned int file_pos = bmpImageoffset;

  uint8_t sdbuffer[3 * BUFFPIXEL];  // 3 * pixels to buffer
  uint8_t buffidx = 3*BUFFPIXEL;
  uint8_t sdbuffer2[3 * BUFFPIXEL];  // 3 * pixels to buffer
  //set up the 'display window'
  tft.setAddrWindow(0, 0, bmpWidth-1, bmpHeight-1);

  for (i=0; i< bmpHeight; i++) {
    // bitmaps are stored with the BOTTOM line first so we have to move 'up'

    for (j=0; j<bmpWidth; j++) {
      // read more pixels
      if (buffidx >= 3*BUFFPIXEL) {
        bmpFile1.read(sdbuffer, 3*BUFFPIXEL);
        bmpFile2.read(sdbuffer2, 3*BUFFPIXEL);
        buffidx = 0;
      }
      // convert pixel from 888 to 565
      b = sdbuffer[buffidx++];     // blue
      g = sdbuffer[buffidx++];     // green
      p = sdbuffer[buffidx++];     // red
      // convert pixel from 888 to 565
      buffidx -= 3;  

      b2 = sdbuffer2[buffidx++];     // blue
      g2 = sdbuffer2[buffidx++];     // green
      p2 = sdbuffer2[buffidx++];     // red

      file_pos += 3;

      float avg_b = ((float)b*ratio )+ ((float)b2*(1-ratio));
      float avg_g = ((float)g*ratio )+ ((float)g2*(1-ratio));
      float avg_r = ((float)p*ratio )+ ((float)p2*(1-ratio));

      b = (int) avg_r;
      g = (int) avg_g;
      p = (int) avg_b;

      p >>= 3;
      p <<= 6;

      g >>= 2;
      p |= g;
      p <<= 5;

      b >>= 3;
      p |= b;

      tft.drawPixel(j, bmpHeight - i, p);
    }
    if( digitalRead(jg.joystick_btn_pin) == HIGH){
      reset_image(&img_index1, &bmpFile1, file_pos); 
    }
    if(digitalRead(jg.btn_pin) == LOW) {
      reset_image(&img_index2, &bmpFile2, file_pos); 
    }
    float cur_val = analogRead(0);
    ratio = cur_val / 1024.0;
  }
}

void reset_image(int *index, File *a_file, int file_pos)
{
  *index = ++*index % num_images;
  a_file->close();
  String img2 = "face";
  img2 += String(*index);
  img2 += ".bmp";
  char  img2c[11] ;
  for(int i = 0; i < 10; i++){
    img2c[i] = img2.charAt(i);
  }
  if (*index < 10 ) {
    for(int i = 10; i >= 5; i--){
      img2c[i] = img2c[i-1];
    }
    img2c[4] = '0';
  }
  img2c[10] = 0;
  *a_file = SD.open(img2c);
  a_file->seek(file_pos); 
}

boolean bmpReadHeader(File f, int mode) {
  // read header
  uint32_t tmp;

  if (read16(f) != 0x4D42) {
    // magic bytes missing
    return false;
  }

  // read file size
  tmp = read32(f);  
  //Serial.print("size 0x"); Serial.println(tmp, HEX);

  // read and ignore creator bytes
  read32(f);

  if (mode == 0 ) bmpImageoffset = read32(f); 
  else if (mode == 1 ) bmpImageoffset2 = read32(f);  
  //Serial.print("offset "); Serial.println(bmpImageoffset, DEC);

  // read DIB header
  tmp = read32(f);
  //Serial.print("header size "); Serial.println(tmp, DEC);
  if (mode == 0 ) {
    bmpWidth = read32(f);
    bmpHeight = read32(f);
  } 
  else if (mode == 1 ) {
    bmpWidth2 = read32(f);
    bmpHeight2 = read32(f);
  }

  if (read16(f) != 1)
    return false;

  if (mode == 0 ) bmpDepth = read16(f);
  else if (mode == 1 ) bmpDepth2 = read16(f);
  //Serial.print("bitdepth "); Serial.println(bmpDepth, DEC);

  if (read32(f) != 0) {
    // compression not supported!
    return false;
  }
  return true;
}

/*********************************************/

// These read data from the SD card file and convert them to big endian 
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t read16(File f) {
  uint16_t d;
  uint8_t b;
  b = f.read();
  d = f.read();
  d <<= 8;
  d |= b;
  return d;
}

// LITTLE ENDIAN!
uint32_t read32(File f) {
  uint32_t d;
  uint16_t b;

  b = read16(f);
  d = read16(f);
  d <<= 16;
  d |= b;
  return d;
}



