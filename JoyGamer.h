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

//#define USE_ACCELEROMETER
#define USE_COLOR

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library

#ifdef USE_COLOR
#include <Color.h>
#endif

#include <SD.h>

#ifdef USE_ACCELEROMETER
#include <I2C.h>
#include <MMA8453Q.h>
#endif

class JoyGamer
{
  public:
    JoyGamer();
    JoyGamer(Adafruit_ST7735 *a_tft);

    void initialize();
    
    #ifdef USE_ACCELEROMETER
    void show_accelerometer(int ax, int ay);
    MMA8453Q accelerometer;
    #endif

    #ifdef USE_COLOR
    void clear_screen(Color=Color(255,255,255));
    #else
    void clear_screen(uint16_t=0);
    #endif
    void draw_string(int x, int y, String text, uint16_t color, int text_size);
    void draw_string(int x, int y, String text, uint16_t color);
    void draw_string(int x, int y, String text);
    void print_integer(int ax, int ay, int to_print, int text_size, boolean=false);
    void play_tone(int tone, int duration);
    void play_note(char note, int duration);
    
    void start_sd_card();
    void bmp_load_and_draw_image(String filename);
    boolean bmp_read_header(File f);
    void bmp_draw(File bitmap);
    
    #ifdef USE_COLOR
    Color background, stroke, fill;
    #endif
    
    int speaker_pin, joystick_btn_pin, btn_pin, joy_x, joy_y, sd_cs;

  private:
    uint32_t read32(File f); 
    uint16_t read16(File f);
    int bmpWidth, bmpHeight;
    uint8_t bmpDepth, bmpImageoffset;
    Adafruit_ST7735 *tft;
    

};

#endif