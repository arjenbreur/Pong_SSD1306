/*
 Eriestuff_Pong_SSD1306 
 Library for displaying and playing Pong on an Adafruit SSD1306 128x64 OLED display.
 Created by Arjen Breur, Jan 2016.
 Code adapted from: http://michaelteeuw.nl/post/87381052117/building-pong

 *** Manual setup required ***
 Open the Adafruit_SSD1306.h file, and add these lines to the top of the file:

    #ifndef Adafruit_SSD1306_h
    #define Adafruit_SSD1306_h
    
 Add this line at the end of the file:
 
    #endif
    
  This is standard practise Arduino code to prevent libraries from being included twice. 
 
 */

#ifndef Eriestuff_Pong_SSD1306_h
#define Eriestuff_Pong_SSD1306_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Adafruit_SSD1306.h>

#define BEEPER 10
#define CONTROL_A A1
#define CONTROL_B A1
#define FONT_SIZE 2
#define SCREEN_WIDTH 127  //real size minus 1, because coordinate system starts with 0
#define SCREEN_HEIGHT 63  //real size minus 1, because coordinate system starts with 0
#define PADDLE_WIDTH 4
#define PADDLE_HEIGHT 10
#define PADDLE_PADDING 5
#define BALL_SIZE 3
#define SCORE_PADDING 10
#define EFFECT_SPEED 0.5
#define MIN_Y_SPEED 0.5
#define MAX_Y_SPEED 2
#define LOOP_DELAY 15

class Eriestuff_Pong_SSD1306{
    public:
        Eriestuff_Pong_SSD1306(Adafruit_SSD1306& display, SmoothAnalogInput& controlA, SmoothAnalogInput& controlB);
        void loop();
        bool isPlaying();
        bool isGameOver();

        void startGame();
        void pauseGame();
        void resumeGame();
        void stopGame();

        void wipeScreen();
        
        Adafruit_SSD1306& _display;
        SmoothAnalogInput& _controlA;
        SmoothAnalogInput& _controlB;   

    private:
        void calculateMovement();
        void draw();
        void addEffect(int paddleSpeed);
        void centerPrint(char *text, int y, int size);
        void gameOver();
        void initNewGame();
        int getRobotPaddleLocation();

        void soundBounce();
        void soundPoint();
        void soundStart();
        void beep(unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds);

        // pong globals
        int controlAMax;
        int controlAMin;
        int controlBMax;
        int controlBMin;
        int paddleLocationA;
        int paddleLocationB;
        float ballX;
        float ballY;
        float ballSpeedX;
        float ballSpeedY;
        int lastPaddleLocationA;
        int lastPaddleLocationB;
        int scoreA;
        int scoreB;
        int pongCountDown;
        int winningScore;
        
        bool _isPlaying;
        bool _isGameOver;
};

#endif
