/*
 Eriestuff_Pong_SSD1306 
 Library for displaying and playing Pong on an Adafruit SSD1306 128x64 OLED display.
 Created by Arjen Breur, Jan 2016.
 Code adapted from: http://michaelteeuw.nl/post/87381052117/building-pong
 */

#include <Adafruit_SSD1306.h>
#include <SmoothAnalogInput.h>
#include "Eriestuff_Pong_SSD1306.h"

// Constructor - Call this *before* the Arduino Setup() function
Eriestuff_Pong_SSD1306::Eriestuff_Pong_SSD1306(Adafruit_SSD1306& display, SmoothAnalogInput& controlA, SmoothAnalogInput& controlB) : _display(display), _controlA(controlA), _controlB(controlB) {
    initNewGame();
    winningScore = DEFAULT_WINNNG_SCORE;
    _isPlaying = false;
}

// PUBLIC METHODS //////////////////////////////////////////////////

// Setup - Call this method from within the Arduino Setup() function
void Eriestuff_Pong_SSD1306::setSoundOutputPin(byte pin){
    _soundOutputPin = pin;
    pinMode(_soundOutputPin,OUTPUT);  // Setup the beeper GPIO as output
}

// Loop - Call this method from within the Arduino Loop() function
void Eriestuff_Pong_SSD1306::loop(){
    if(_isPlaying){
        if(max(scoreA, scoreB) >= winningScore){
            gameOver();
        }else{
            if(pongCountDown == 0){
                calculateMovement();
                draw();
                delay(LOOP_DELAY);
            }else{
                // abuse scoreboard for countdown
                scoreA = pongCountDown;
                scoreB = pongCountDown;
                draw();
                pongCountDown--;
                delay(1000);
            }
        }
    }
}

bool Eriestuff_Pong_SSD1306::isPlaying(){
    return _isPlaying;
}

bool Eriestuff_Pong_SSD1306::isGameOver(){
    return _isGameOver;
}

void Eriestuff_Pong_SSD1306::startGame(){
    _isPlaying = true;
    _isGameOver = false;
    initNewGame();
    wipeScreen();
}

void Eriestuff_Pong_SSD1306::stopGame(){
    _isPlaying = false;
    _isGameOver = false;
}

void Eriestuff_Pong_SSD1306::wipeScreen(void) {
    for (int16_t i=0; i<_display.height()/2; i+=1) {
        _display.drawRect(i, i, _display.width()-2*i, _display.height()-2*i, WHITE);
        _display.display();
    }
    for (int16_t i=_display.height()/2; i>=-1; i-=1) {
        _display.drawRect(i, i, _display.width()-2*i, _display.height()-2*i, BLACK);
        _display.display();
    }
}

// PRIVATE METHODS ////////////////////////////////////////////////

void Eriestuff_Pong_SSD1306::gameOver(){
    _isGameOver = true;
    // display winner
    wipeScreen();
    _display.setCursor(3, 20);
    _display.setTextSize(3);
    if(scoreA > scoreB) _display.println("A Wins!");
    else                _display.println("B Wins!");
    _display.display();
    delay(1000);
}

void Eriestuff_Pong_SSD1306::initNewGame(){
    //reset
    scoreA = 0;
    scoreB = 0;
    controlAMax = 0;    // init to minimum
    controlAMin = 1023; // init to maximum
    controlBMax = controlAMax;
    controlBMin = controlAMin;
    pongCountDown = COUNT_DOWN;
    paddleLocationA = 0;
    paddleLocationB = 0;
    lastPaddleLocationA = 0;
    lastPaddleLocationB = 0;
    ballX = SCREEN_WIDTH/2;
    ballY = SCREEN_HEIGHT/2;
    ballSpeedX = random(0,2) < 1 ? -2 : 2;
    ballSpeedY = random(-2,2);
}

void Eriestuff_Pong_SSD1306::calculateMovement(){
    boolean isARobot = false; // TODO: getRobotPaddleLocation() code is not ready to calculate controlA.
    boolean isBRobot = true;

    int controlAValue = _controlA.read();
    int controlBValue = _controlB.read();

    // take 70% of reading as maximum reading for the controls
    controlAMax = max(controlAMax, controlAValue*0.7); 
    controlAMin = min(controlAMin, controlAValue*1.3);
    controlBMax = max(controlBMax, controlBValue*0.7);
    controlBMin = min(controlBMin, controlBValue*1.3);

    byte maxPaddleLocation = SCREEN_HEIGHT - PADDLE_HEIGHT;

    if(isARobot){
        paddleLocationA = getRobotPaddleLocation();
    }else{
        paddleLocationA = map(controlAValue, controlAMax, controlAMin, 0, maxPaddleLocation);
        if (paddleLocationA > maxPaddleLocation) paddleLocationA = maxPaddleLocation;
        if (paddleLocationA < 0) paddleLocationA = 0;
    }

    if(isBRobot){
        paddleLocationB = getRobotPaddleLocation();
    }else{
        paddleLocationB = map(controlBValue, controlBMax, controlBMin, 0, maxPaddleLocation);
        if (paddleLocationB > maxPaddleLocation) paddleLocationB = maxPaddleLocation;
        if (paddleLocationB < 0) paddleLocationB = 0;
    }

    byte paddleSpeedA = paddleLocationA - lastPaddleLocationA;
    byte paddleSpeedB = paddleLocationB - lastPaddleLocationB;

    ballX += ballSpeedX;
    ballY += ballSpeedY;

    //bounce from top and bottom
    if (ballY >= SCREEN_HEIGHT - BALL_SIZE || ballY <= 0) {
        ballSpeedY *= -1;
        soundBounce();
    }

    //bounce from paddle A
    if (ballX >= PADDLE_PADDING && ballX <= PADDLE_PADDING+BALL_SIZE && ballSpeedX < 0) {
        if (ballY > paddleLocationA - BALL_SIZE && ballY < paddleLocationA + PADDLE_HEIGHT) {
            soundBounce();
            ballSpeedX *= -1;
            addEffect(paddleSpeedA);
        }
    }

    //bounce from paddle B
    if (ballX >= SCREEN_WIDTH-PADDLE_WIDTH-PADDLE_PADDING-BALL_SIZE && ballX <= SCREEN_WIDTH-PADDLE_PADDING-BALL_SIZE && ballSpeedX > 0) {
        if (ballY > paddleLocationB - BALL_SIZE && ballY < paddleLocationB + PADDLE_HEIGHT) {
        soundBounce();
        ballSpeedX *= -1;
        addEffect(paddleSpeedB);
        }
    }

    //score points if ball hits wall behind paddle
    if (ballX >= SCREEN_WIDTH - BALL_SIZE || ballX <= 0) {
        if (ballSpeedX > 0) {
            scoreA++;
            ballX = SCREEN_WIDTH / 4;
        }
        if (ballSpeedX < 0) {
            scoreB++;
            ballX = SCREEN_WIDTH / 4 * 3;
        }
        soundPoint();   
    }

    //set last paddle locations
    lastPaddleLocationA = paddleLocationA;
    lastPaddleLocationB = paddleLocationB;  
}

void Eriestuff_Pong_SSD1306::draw(){
    _display.clearDisplay(); 

    //draw paddle A
    _display.fillRect(PADDLE_PADDING,paddleLocationA,PADDLE_WIDTH,PADDLE_HEIGHT,WHITE);

    //draw paddle B
    _display.fillRect(SCREEN_WIDTH-PADDLE_WIDTH-PADDLE_PADDING,paddleLocationB,PADDLE_WIDTH,PADDLE_HEIGHT,WHITE);

    //draw center line
    for (byte i=0; i<SCREEN_HEIGHT; i+=4) {
        _display.drawFastVLine(SCREEN_WIDTH/2, i, 2, WHITE);
    }

    //draw ball
    _display.fillRect(ballX,ballY,BALL_SIZE,BALL_SIZE,WHITE);

    //print scores

    //backwards indent score A. This is dirty, but it works ... ;)
    byte scoreAWidth = 5 * FONT_SIZE;
    if (scoreA > 9) scoreAWidth += 6 * FONT_SIZE;
    if (scoreA > 99) scoreAWidth += 6 * FONT_SIZE;
    if (scoreA > 999) scoreAWidth += 6 * FONT_SIZE;
    if (scoreA > 9999) scoreAWidth += 6 * FONT_SIZE;

    _display.setCursor(SCREEN_WIDTH/2 - SCORE_PADDING - scoreAWidth,0);
    _display.print(scoreA);

    _display.setCursor(SCREEN_WIDTH/2 + SCORE_PADDING+1,0); //+1 because of dotted line.
    _display.print(scoreB);

    _display.display();
}  

//add effect to the ball
void Eriestuff_Pong_SSD1306::addEffect(byte paddleSpeed){
    float oldBallSpeedY = ballSpeedY;

    //add effect to ball when paddle is moving while bouncing.
    //for every pixel of paddle movement, add or substact EFFECT_SPEED to ballspeed.
    for (byte effect = 0; effect < abs(paddleSpeed); effect++) {
        if (paddleSpeed > 0) {
            ballSpeedY += EFFECT_SPEED;
        } else {
            ballSpeedY -= EFFECT_SPEED;
        }
    }

    //limit to minimum speed
    if (ballSpeedY < MIN_Y_SPEED && ballSpeedY > -MIN_Y_SPEED) {
        if (ballSpeedY > 0) ballSpeedY = MIN_Y_SPEED;
        if (ballSpeedY < 0) ballSpeedY = -MIN_Y_SPEED;
        if (ballSpeedY == 0) ballSpeedY = oldBallSpeedY;
    }

    //limit to maximum speed
    if (ballSpeedY > MAX_Y_SPEED) ballSpeedY = MAX_Y_SPEED;
    if (ballSpeedY < -MAX_Y_SPEED) ballSpeedY = -MAX_Y_SPEED;
}

void Eriestuff_Pong_SSD1306::centerPrint(char *text, byte y, byte size){
    _display.setTextSize(size);
    _display.setCursor(SCREEN_WIDTH/2 - ((strlen(text))*6*size)/2,y);
    _display.print(text);
}

// TODO: getRobotPaddleLocation() code is not ready to calculate controlA.
byte Eriestuff_Pong_SSD1306::getRobotPaddleLocation(){
    byte paddleSpeed = 0;

    // x location of paddle face
    byte paddleX = SCREEN_WIDTH-PADDLE_WIDTH-PADDLE_PADDING;
    // distance ball needs to travel in x direction until it hits paddle
    byte diffX = paddleX - ballX;

    if(ballSpeedX < 0 || diffX <=0){
        // ball is moving from B to A
        // or ball is beyond paddle B
    }else{
        // ball is moving towards paddle B
        // try to intercept it
        // number of iterations needed for ball to travel this distance in x direction   
        float cyclesToGo = abs(diffX)/ballSpeedX;
        // total distance in y direction ball will travel during these iterations (until ball hits paddle)
        byte predictedYWithoutBounce = ballY + (cyclesToGo * ballSpeedY); // will be negative when ball is moving up out of the screen.
        // distance in y direction left, after last bounce (from TOP or BOTTOM of screen)
        byte predictedY = predictedYWithoutBounce;
        // unless there is a bounce:
        if(predictedYWithoutBounce > SCREEN_HEIGHT){
            // ball will bounce (at least once) on bottom of screen
            predictedY = SCREEN_HEIGHT - (predictedYWithoutBounce - SCREEN_HEIGHT);
        }
        if(predictedYWithoutBounce < 0){
            // ball will bounce from top of screen
            predictedY = predictedYWithoutBounce *-1;
        }
        // set paddleSpeed to get to the predicted y location in time
        byte diffY = abs(lastPaddleLocationB - predictedY);
        paddleSpeed = diffY / cyclesToGo;
        if(predictedY < lastPaddleLocationB){
            paddleSpeed *=-1; // move up!
        }
    }
  
    // paddleSpeed is now perfect... resulting in a perfect Pong robot
    // but we want the robot to make mistakes...

    // use same random number seed while score is the same 
    // so, random numbers generated with random() will be the same for the duration of this point
    randomSeed(scoreA+scoreB);

    // limit the maximum speed to 1
    if(paddleSpeed>0) paddleSpeed = 1;
    if(paddleSpeed<0) paddleSpeed =-1;

    // start moving too late, don't react until the ball is somewhere between half and 1/3rd of the screen.
    byte reactionDistance = random(SCREEN_WIDTH/2, SCREEN_WIDTH/3);
    if(diffX > reactionDistance) paddleSpeed = 0;

    // hesitate now and then
    if(random(0,3) == 0) paddleSpeed = 0;

    byte paddleLocation = lastPaddleLocationB + paddleSpeed;
    return paddleLocation; 
}

// SOUND METHODS //////////////////////////////////////////////////////////////

void Eriestuff_Pong_SSD1306::soundBounce(){ beep(_soundOutputPin, 500, 50);}

void Eriestuff_Pong_SSD1306::soundPoint(){  beep(_soundOutputPin, 150, 150);}

void Eriestuff_Pong_SSD1306::soundStart(){
    beep(_soundOutputPin, 250, 100);
    delay(100);
    beep(_soundOutputPin, 500, 100);
    delay(100);
    beep(_soundOutputPin, 1000, 100);
    delay(100);
    noTone(_soundOutputPin);
}

// the sound producing function
// http://web.media.mit.edu/~leah/LilyPad/07_sound_code.html
void Eriestuff_Pong_SSD1306::beep(unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds){
    int x;   
    long delayAmount = (long)(1000000/frequencyInHertz);
    long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
    for (x=0;x<loopTime;x++){  
        if(speakerPin>-1) digitalWrite(speakerPin,HIGH);
        delayMicroseconds(delayAmount);
        if(speakerPin>-1) digitalWrite(speakerPin,LOW);
        delayMicroseconds(delayAmount);
    }  
}
