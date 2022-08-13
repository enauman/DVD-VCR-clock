//LED display from a DVD/VCR, running on Metro Mini
//made into clock w laser cut clear acrylic cover
//custom milled circuit board
#include <NSegmentDisplay.h>
#include <TimeLib.h>

//common cathode display with 12 segments
const int NUM_SEG_PINS = 12;
const int NUM_DIGIT_PINS = 5;
const int segments[] = {13, 5, 8, 2, 7, 6, 4, 12, 9, 11, 10, 3};
const int digits[] = {A1, A2, A3, A5, A4};
const int button = A0;
boolean pressed;
NSegmentDisplay disp(false, NUM_SEG_PINS, segments, NUM_DIGIT_PINS, digits);
//time variables
int hr = 12;
int mn = 00;
int waitTime = 250;
double lastTime = 0;
//data for animation each minute 
const uint8_t paths[8][3][2] {
  {{3, 9}, {4, 9}, {4, 10}}, // left side (0)
  {{4, 0}, {4, 7}, {4, 3}},
  {{3, 0}, {4, 6}, {3, 3}}, //4th digit
  {{2, 0}, {3, 6}, {2, 3}}, //3rd digit
  {{1, 0}, {2, 6}, {1, 3}}, //2nd digit
  {{0, 0}, {1, 6}, {0, 3}}, //1st digit
  {{2, 9}, {0, 6}, {0, 10}}, //0 digit
  {{2, 10}, {0, 9}, {1, 10}}//right side (20)
};
const uint8_t walls[15][2][2] {
  {{4, 5}, {4, 4}}, //4th digit
  {{4, 5}, {4, 8}},
  {{4, 1}, {4, 2}},
  {{3, 5}, {3, 4}}, //3rd digit
  {{3, 7}, {3, 8}},
  {{3, 1}, {3, 2}},
  {{2, 5}, {2, 4}}, //2nd digit
  {{2, 7}, {2, 8}},
  {{2, 1}, {2, 2}},
  {{1, 5}, {1, 4}}, //1st digit
  {{1, 9}, {1, 8}},
  {{1, 1}, {1, 2}},
  {{0, 5}, {0, 4}}, //0 digit
  {{0, 7}, {0, 8}},
  {{0, 1}, {0, 2}}
};
//segments for each second tick
const int ticks[9][2] {
  {4, 10}, {4, 9}, {3, 9}, {3, 10}, {2, 9}, {2, 10}, {0, 9}, {0, 10}, {1, 10}
};

void setup() {
  setTime(hr, mn, 0, 1, 1, 20); // set time to 12:00:00am Jan 1 2020
  pinMode(button, INPUT_PULLUP);
}

void loop() {
  pressed = digitalRead(button);
  if (!pressed) {
    if (millis() > waitTime / 5 + lastTime) {
      updateMin();
      lastTime = millis();
    }
  }
  while (second() < 2.5) {
    wholeGrid();
  }
  minuteCorrection();
  doTicks();
  clockTime();
}

int tickCounter = 0;
void doTicks() {
  disp.segment(ticks[tickCounter][0], ticks[tickCounter][1], 3);
  if (millis() > waitTime * 4 + lastTime) {
    tickCounter++;
    lastTime = millis();
    if (tickCounter > 8) tickCounter = 0;
  }
}

void updateMin() {
  mn = minute() + 1;
  setTime(hourFormat12(), mn, second(), 1, 1, 20);
}

void clockTime() {
  disp.multiDigitNumber(2, hourFormat12());
  //colons
  disp.segment(2, 11, 3);
  disp.multiDigitNumber(minute());
  if (minute() < 10) disp.number(1, 0);
}

void wholeGrid() {
  for (int j = 0; j < 8; j++) {
    minuteCorrection();
    disp.segment(paths[j][0][0], paths[j][0][1], 100);
  }
  for (int j = 0; j < 15; j++) {
    minuteCorrection();
    disp.segment(walls[j][0][0], walls[j][0][1], 50);
  }
  for (int j = 0; j < 8; j++) {
    minuteCorrection();
    disp.segment(paths[j][1][0], paths[j][1][1], 100);
  }
  for (int j = 0; j < 15; j++) {
    disp.segment(walls[j][1][0], walls[j][1][1], 50);
  }
  for (int j = 0; j < 8; j++) {
    disp.segment(paths[j][2][0], paths[j][2][1], 100);
  }
}

//clock gains ~2min per day so this sets it back 1 min each 12 hrs
bool hasCorrected = false;
void minuteCorrection() {
  if (hourFormat12() == 3 && minute() == 15 && second() == 1 && !hasCorrected) {
    mn = minute() - 1;
    setTime(hourFormat12(), mn, second(), 1, 1, 20);
    hasCorrected = true;
  }
  if (minute() == 16) hasCorrected = false;
}
