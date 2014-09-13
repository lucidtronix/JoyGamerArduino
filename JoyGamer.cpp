/*
 *  JoyGamer.cpp
 *  
 *
 *  Created by Samwell Freeman on 7/20/14.
 *  Copyright 2014 Bdau Bdow. All rights reserved.
 *
 */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "JoyGamer.h"

JoyGamer :: JoyGamer()
{

}

JoyGamer :: JoyGamer(Adafruit_ST7735 *a_tft)
{
  tft = a_tft;
  speaker_pin = 5;
  btn_pin = 12;
  joy_x = 0;
  joy_y = 1;
  sd_cs = 4;

}

void JoyGamer :: initialize()
{
  pinMode(btn_pin, INPUT);
  pinMode(speaker_pin, OUTPUT); 
  tft->initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab
  stroke = Color(30, 34, 30);
  background = Color(240, 240, 240);
  tft->fillScreen(background.color_16());
  tft->setTextColor(stroke.color_16());
}

void JoyGamer :: start_sd_card()
{
  if (!SD.begin(sd_cs)) {
    draw_string(12, 42, "SD Failed", Color(255,0,0).color_16());
    return;
  }
  draw_string(12,42, "SD GOOD", Color(0,255,0).color_16());
}

void JoyGamer :: show_accelerometer(int ax, int ay)
{
  tft->fillRect(ax, ay, 50, 40, ST7735_WHITE);
  tft->setCursor(ax, ay);
  tft->print("X:");
  print_integer(ax+15, ay, accelerometer.getX(), 1, false);
  
  tft->setCursor(ax, ay + 10);
  tft->print("Y:");
  print_integer(ax+15, ay+10, accelerometer.getY(), 1, false);
  
  tft->setCursor(ax, ay + 20);
  tft->print("Z:");
  print_integer(ax+15, ay+20, accelerometer.getZ(), 1, false);
}


void JoyGamer :: play_tone(int tone, int duration) 
{
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speaker_pin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speaker_pin, LOW);
    delayMicroseconds(tone);
  }
}

void JoyGamer :: play_note(char note, int duration) 
{
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      play_tone(tones[i], duration);
    }
  }
}

void JoyGamer :: clear_screen(Color c)
{
  // clear weird edge pixels
  tft->fillRect(-2, -2, 5, 161, c.color_16());
  tft->fillRect(-2, -2, 131, 5, c.color_16());
  tft->fillScreen(c.color_16());
}


void JoyGamer :: draw_string(int x, int y, String text, uint16_t color, int text_size)
{
  tft->setTextSize(text_size);
  draw_string(x, y, text, color);
}

void JoyGamer :: draw_string(int x, int y, String text, uint16_t color)
{
  tft->setTextColor(color);
  tft->setCursor(x, y);
  tft->print(text);
}

void JoyGamer :: draw_string(int x, int y, String text)
{
  tft->setTextColor(stroke.color_16());
  tft->setCursor(x, y);
  tft->print(text);
}

void JoyGamer :: print_integer(int ax, int ay, int to_print, int text_size, boolean hex=false)
{
  tft->setTextColor(stroke.color_16());
  tft->setTextSize(text_size);
  tft->setCursor(ax, ay);
  if(hex) tft->print(String(to_print, HEX));
  else tft->print(String(to_print));
}

void JoyGamer :: bmp_load_and_draw_image(String filename) 
{
  char char_array[16];
  filename.toCharArray(char_array, filename.length()+1);
  bmpFile1 = SD.open(char_array);

  if (!bmpFile1) {
    draw_string(12, 62, "No image", Color(255,0,0).color_16());
    draw_string(12, 72, filename, Color(255,0,0).color_16());
    return;
  }
  
  if (!bmp_read_header(bmpFile1,0)) { 
     draw_string(12, 82, "Bad image", Color(255,0,0).color_16());
     draw_string(12, 92, filename, Color(255,0,0).color_16());
     return;
  }
  bmpdraw();
  bmpFile1.close();
}


#define BUFFPIXEL 20
void JoyGamer :: bmpdraw() 
{
  bmpFile1.seek(bmpImageoffset);
  unsigned int file_pos = bmpImageoffset;
  uint32_t time = millis();
  uint16_t p; 
  uint8_t g, b;
  int i, j;
  
  uint8_t sdbuffer[3 * BUFFPIXEL];  // 3 * pixels to buffer
  uint8_t buffidx = 3*BUFFPIXEL;
  
  
  for (i=0; i< bmpHeight; i++) {
    // bitmaps are stored with the BOTTOM line first so we have to move 'up'
  
    for (j=0; j<bmpWidth; j++) {
      // read more pixels
      if (buffidx >= 3*BUFFPIXEL) {
        bmpFile1.read(sdbuffer, 3*BUFFPIXEL);
        buffidx = 0;
      }
      
      // convert pixel from 888 to 565
      p = sdbuffer[buffidx++];     // blue
      g = sdbuffer[buffidx++];     // green
      b = sdbuffer[buffidx++];     // red
      
      file_pos += 3;
      
      p >>= 3;
      p <<= 6;
      
      g >>= 2;
      p |= g;
      p <<= 5;
      
      b >>= 3;
      p |= b;
      // write out the 16 bits of color
      tft->drawPixel(bmpWidth-j, bmpHeight- i, p);
    }
  }
}

boolean JoyGamer :: bmp_read_header(File f, int mode) 
{
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
  
  bmpImageoffset = read32(f); 
  //Serial.print("offset "); Serial.println(bmpImageoffset, DEC);
  
  // read DIB header
  tmp = read32(f);
  //Serial.print("header size "); Serial.println(tmp, DEC);
  bmpWidth = read32(f);
  bmpHeight = read32(f);
  

  
  if (read16(f) != 1)
    return false;
    
  bmpDepth = read16(f);
  //Serial.print("bitdepth "); Serial.println(bmpDepth, DEC);

  if (read32(f) != 0) {
    // compression not supported!
    return false;
  }
  
  //Serial.print("compression "); Serial.println(tmp, DEC);

  return true;
}

/*********************************************/

// These read data from the SD card file and convert them to big endian 
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t JoyGamer :: read16(File f) 
{
  uint16_t d;
  uint8_t b;
  b = f.read();
  d = f.read();
  d <<= 8;
  d |= b;
  return d;
}


// LITTLE ENDIAN!
uint32_t JoyGamer :: read32(File f) 
{
  uint32_t d;
  uint16_t b;
 
  b = read16(f);
  d = read16(f);
  d <<= 16;
  d |= b;
  return d;
}

