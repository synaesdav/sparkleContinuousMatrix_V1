//based on sparkleExploreMatrix_V4
//basic extrapolation of just the sparkle functions (no background or text scrolling code included)

//best version for continuous sparkles on a matrix
//added gamma correction to sparkles and background
//added set or random color controll of sparkles
//in setSparkle(colorStyle) use 0 for set color, 1 for multi-colored
//added scrolling text overlay using textScrolling_V2
//converted sparkleExplore_V5 to work on a matrix, condensed into functions
//improved color manipulation for background and sparkles
//set the color for the background and sparkles in the header
//re-randomize the timing after each sparkle
//tweak timing by changing value ranges in getRandom()
//moving color wash as background
//makes multiple sparkles using arrays
//using randomized timing for more organic sparkles
//A sketch to explore using timing with millis to create a sparkle effect
//sparkle is broken up into 3 parts:ramp up, hold, and trail off.
//Each part has variable timing

//for Circuit Playground Classic or Feather M4 Express
//written by David Crittenden 1/2023


#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <Adafruit_NeoMatrix_ZeroDMA.h>

#define PIN 12 //pin 6 does not work with zeroDMA

#define mxWidth 8 //change this if matrix size changes
#define mxHeight 8 //change this if matrix size changes

Adafruit_NeoMatrix_ZeroDMA matrix(mxWidth, mxHeight, PIN,
                                  NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                                  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                                  NEO_GRB         + NEO_KHZ800);


//for loop manipulation
uint8_t matrixWide;
uint8_t matrixTall;

unsigned long currentMillis = millis();//holds the current time

//some variables for the SPARKLES
//set the color of the sparkles here
uint8_t sparkleRed = 247;
uint8_t sparkleGreen = 247;
uint8_t sparkleBlue = 166;

uint8_t colorStyle;//0 for set color, 1 for random color

//variables for manipulating sparkle color
int R[mxWidth][mxHeight];
int G[mxWidth][mxHeight];
int B[mxWidth][mxHeight];

uint8_t stage[mxWidth][mxHeight];//variable to hold what stage the sparkle is in
int flashBright[mxWidth][mxHeight];//variable to hold light level of the sparkle

unsigned long rampUpPreviousMillis[mxWidth][mxHeight];
int rampUpInterval[mxWidth][mxHeight];//timing for the ramp up
uint8_t rampUpScaler[mxWidth][mxHeight];//number to add to flashBright each iteration loop

unsigned long holdPreviousMillis[mxWidth][mxHeight];
int holdInterval[mxWidth][mxHeight];//timing for the hold

unsigned long trailOffPreviousMillis[mxWidth][mxHeight];
int trailOffInterval[mxWidth][mxHeight];//timing for the trail off
uint8_t trailOffScaler[mxWidth][mxHeight];//number to subtract from flashBright each iteration loop

unsigned long pausePreviousMillis[mxWidth][mxHeight];
int pauseInterval[mxWidth][mxHeight];//timing for the pause between sparkles

//*************************************************************************************
void setup()
{
  matrix.begin();
  matrix.setBrightness(50);
  matrix.show();

  randomSeed(analogRead(0));//initialize the random function

  //FOR THE SPARKLES - fill the initial arrays with values
  for (matrixWide = 0; matrixWide < matrix.width(); matrixWide++)
  {
    for (matrixTall = 0; matrixTall < matrix.height(); matrixTall++)
    {
      getRandom(colorStyle);//fill the arrays with values
      stage[matrixWide][matrixTall] = 4;//start with the pause stage to offset the beginning of all the sparkles
    }
  }
}
//**************************************************************************************
void loop()
{
  currentMillis = millis();//check the current time

  matrix.fillScreen(0);//colorless background

  setSparkles(1);//0 for set color, 1 for multi color
  
  matrix.show();//show the data sent to the pix
}
//**************************************************************************************

void getRandom(uint8_t colorStyle)//fill the arrays with randomized values
{
  rampUpInterval[matrixWide][matrixTall] = random(1, 10);
  rampUpScaler[matrixWide][matrixTall] = random(5, 20);
  holdInterval[matrixWide][matrixTall] = random(10, 100);
  trailOffInterval[matrixWide][matrixTall] = random(1, 20);
  trailOffScaler[matrixWide][matrixTall] = random(5, 10);
  pauseInterval[matrixWide][matrixTall] = random(500, 5000);

  stage[matrixWide][matrixTall] = 1;//return to step one

  if (colorStyle == 0)//set sparkle color
  {
    R[matrixWide][matrixTall] = sparkleRed;
    G[matrixWide][matrixTall] = sparkleGreen;
    B[matrixWide][matrixTall] = sparkleBlue;
  }

  if (colorStyle == 1)//random sparkle color
  {
    R[matrixWide][matrixTall] = random(255);
    G[matrixWide][matrixTall] = random(255);
    B[matrixWide][matrixTall] = random(255);
  }
}

void setSparkles(uint8_t colorStyle) //iterate through all pixels and set levels
{
  //iterate through all pixels in the matrix
  for (matrixWide = 0; matrixWide < matrix.width(); matrixWide++)
  {
    for (matrixTall = 0; matrixTall < matrix.height(); matrixTall++)
    {
      //quick ramp up STAGE 1
      if (stage[matrixWide][matrixTall] == 1)
      {
        if (currentMillis - rampUpPreviousMillis[matrixWide][matrixTall] > rampUpInterval[matrixWide][matrixTall])//is it time to advance?
        {
          flashBright[matrixWide][matrixTall] = flashBright[matrixWide][matrixTall] + rampUpScaler[matrixWide][matrixTall];//increace the brightness
          rampUpPreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the timer
          holdPreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the next timer
        }
        if (flashBright[matrixWide][matrixTall] >= 255)//as bright as it gets
        {
          flashBright[matrixWide][matrixTall] = 255;
          stage[matrixWide][matrixTall] = 2;//advance to the next stage
        }
      }

      //hold at full STAGE 2
      if (stage[matrixWide][matrixTall] == 2)
      {
        flashBright[matrixWide][matrixTall] = 255;//hold at max brightness

        if (currentMillis - holdPreviousMillis[matrixWide][matrixTall] > holdInterval[matrixWide][matrixTall])//is it time to advance?
        {
          stage[matrixWide][matrixTall] = 3;//advance to the next stage
          holdPreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the timer
          trailOffPreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the next timer
        }
      }

      //trail off STAGE 3
      if (stage[matrixWide][matrixTall] == 3)
      {
        if (currentMillis - trailOffPreviousMillis[matrixWide][matrixTall] > trailOffInterval[matrixWide][matrixTall])//is it time to advance?
        {
          flashBright[matrixWide][matrixTall] = flashBright[matrixWide][matrixTall] - trailOffScaler[matrixWide][matrixTall];//decreace the brightness
          trailOffPreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the timer
          pausePreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the next timer
        }
        if (flashBright[matrixWide][matrixTall] <= 0)//as dim as it gets
        {
          flashBright[matrixWide][matrixTall] = 0;
          stage[matrixWide][matrixTall] = 4;//advance to the next stage
        }
      }

      if (stage[matrixWide][matrixTall] == 4)//pause between sparkles
      {
        if (currentMillis - pausePreviousMillis[matrixWide][matrixTall] > pauseInterval[matrixWide][matrixTall])//is it time to advance?
        {
          pausePreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the timer
          rampUpPreviousMillis[matrixWide][matrixTall] = currentMillis;//reset the next timer
          getRandom(colorStyle);//change the values in the array
        }
      }
    }
  }
  //send sparkle data to the pix
  for (matrixWide = 0; matrixWide < matrix.width(); matrixWide++)
  {
    for (matrixTall = 0; matrixTall < matrix.height(); matrixTall++)
    {
      if (stage[matrixWide][matrixTall] != 4)//don't send data during the pause stage
      {
        int red = matrix.gamma8(flashBright[matrixWide][matrixTall] * R[matrixWide][matrixTall] / 255);
        int green = matrix.gamma8(flashBright[matrixWide][matrixTall] * G[matrixWide][matrixTall] / 255);
        int blue = matrix.gamma8(flashBright[matrixWide][matrixTall] * B[matrixWide][matrixTall] / 255);
        matrix.drawPixel(matrixWide, matrixTall, matrix.Color(red, green, blue));
      }
    }
  }
}
