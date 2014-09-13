/*
 *  JoyGamer.h
 *  
 *
 *  Created by Samwell Freeman on 7/20/14.
 *  Copyright 2014 LucidTronix.
 *
 */

#ifndef JoyGamer_h
#define JoyGamer_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Color.h>
#include <SD.h>
#include <I2C.h>
#include <MMA8453Q.h>

class JoyGamer
{
  public:
    JoyGamer();
    JoyGamer(Adafruit_ST7735 *a_tft);

    void initialize();
    void show_accelerometer(int ax, int ay);
    
    void clear_screen(Color=Color(0,0,0));

    void draw_string(int x, int y, String text, uint16_t color, int text_size);
    void draw_string(int x, int y, String text, uint16_t color);
    void draw_string(int x, int y, String text);
    void print_integer(int ax, int ay, int to_print, int text_size, boolean hex);
    void play_tone(int tone, int duration);
    void play_note(char note, int duration);
    
    void start_sd_card();
    void bmp_load_and_draw_image(String filename);

    Color background, stroke, fill;
    int speaker_pin, btn_pin, joy_x, joy_y, sd_cs;

  private:
    void bmpdraw();
    boolean bmp_read_header(File f, int mode);
    uint32_t read32(File f); 
    uint16_t read16(File f);
    File bmpFile1;
    int bmpWidth, bmpHeight;
    uint8_t bmpDepth, bmpImageoffset;
    Adafruit_ST7735 *tft;
    MMA8453Q accelerometer;
};

#endif