/*
 Eriestuff_Pong_SSD1306 
 Library for displaying and playing Pong on an Adafruit SSD1306 128x64 OLED display.
 Created by Arjen Breur, Jan 2016.
 Code adapted from: http://michaelteeuw.nl/post/87381052117/building-pong
 */

#ifndef Eriestuff_Pong_SSD1306_h
#define Eriestuff_Pong_SSD1306_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Adafruit_SSD1306.h>

class Eriestuff_Pong_SSD1306{
    public:
        Eriestuff_Pong_SSD1306(Adafruit_SSD1306& display, SmoothAnalogInput& controlA, SmoothAnalogInput& controlB);
        
        void setSoundOutputPin(byte pin);
        void loop();
        bool isPlaying();
        bool isGameOver();

        void startGame();
        void stopGame();

        void wipeScreen();
        
        Adafruit_SSD1306& _display;
        SmoothAnalogInput& _controlA;
        SmoothAnalogInput& _controlB;   

    private:
        void calculateMovement();
        void draw();
        void addEffect(byte paddleSpeed);
        void centerPrint(char *text, byte y, byte size);
        void gameOver();
        void initNewGame();
        byte getRobotPaddleLocation();

        void soundBounce();
        void soundPoint();
        void soundStart();
        void beep(unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds);

        // defines
        const byte SCREEN_WIDTH  = SSD1306_LCDWIDTH  -1;  //real size minus 1, because coordinate system starts with 0
        const byte SCREEN_HEIGHT = SSD1306_LCDHEIGHT -1;  //real size minus 1, because coordinate system starts with 0
        const byte FONT_SIZE =  2;
        const byte PADDLE_WIDTH    =  4;
        const byte PADDLE_HEIGHT   = 10;
        const byte PADDLE_PADDING  =  5;
        const byte BALL_SIZE       =  3;
        const byte SCORE_PADDING   = 10;
        const byte LOOP_DELAY      = 15;
        const byte DEFAULT_WINNNG_SCORE = 11;
        const float EFFECT_SPEED  =  0.5;
        const float MIN_Y_SPEED   =  0.5;
        const float MAX_Y_SPEED   =  2;
        const float COUNT_DOWN = 0;

        // globals
        byte _soundOutputPin = -1; // -1 = no sound

        int controlAMax;
        int controlAMin;
        int controlBMax;
        int controlBMin;
        byte paddleLocationA;
        byte paddleLocationB;
        byte lastPaddleLocationA;
        byte lastPaddleLocationB;
        byte scoreA;
        byte scoreB;
        byte pongCountDown;
        byte winningScore;
        float ballX;
        float ballY;
        float ballSpeedX;
        float ballSpeedY;
        bool _isPlaying;
        bool _isGameOver;
};

#endif
