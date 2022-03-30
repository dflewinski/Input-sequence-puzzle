#include <Arduino.h>

// DEFINES
#define DEBUG

// CONSTANS
// Numbers of posible inputs
const byte numInputs = 3;
// Pins where buttons connected to
const byte inputsPins[numInputs] = {D7, D8, D9};
// Number of steps in the sequence
const byte numSteps = 5;
// Led pins
const byte ledPins[numSteps] = {D2, D3, D4, D5, D6};
// Pin where relay connected to
const byte lockPin = D10;

// GLOBALS
// Correct sequence
byte steps[numSteps];
// Assume the default state of each switch
bool lastInputState[] = {HIGH, HIGH, HIGH, HIGH};
// What step of sequence is passing
int currentStep = 0;

// Debouncing of swiches
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

template <typename C, typename T>
bool contains(C&& c, T e) {
  return std::find(std::begin(c), std::end(c), e) != std::end(c);
};

template <typename C, typename T>
void check(C&& c, T e) {
  return (contains(c, e) ? true : false);
}

void generateSequence() {
  for (size_t i = 0; i < numSteps; i++) {
    steps[i] = (byte)random(0, numInputs);
    delay(10);
  }
  for (size_t i = 0; i < numSteps; i++) {
    Serial.print(steps[i]);
    Serial.print(F(" "));
  }

  // Check the sequence by using all inputs
  for (size_t i = 0; i < numInputs; i++) {
    if (!contains(steps, i)) {
      // The sequence don't have input -> generate a new sequence
      Serial.println(F(" The sequence is not valid, generate one more."));
      generateSequence();
    }
  }
  Serial.println();
}

void onSolve() {
#ifdef DEBUG
  Serial.println(F("Puzzle solved"));
#endif

  // Realase a lock
  digitalWrite(lockPin, LOW);

  // Loop forever
  while (true) {
    // Flash a wave of leds
    for (size_t i = 0; i < numSteps; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(100);
    }

    for (size_t i = 0; i < numSteps; i++) {
      digitalWrite(ledPins[i], LOW);
      delay(100);
    }
  }
}

void setup() {
  // Initialize the input pins
  for (size_t i = 0; i < numInputs; i++) {
    pinMode(inputsPins[i], INPUT_PULLUP);
  }

  // Initialize the led pins
  for (size_t i = 0; i < numSteps; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Set the lock pin to secure mode
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, HIGH);

#ifdef DEBUG
  Serial.begin(9600);
  Serial.println(F("Serial connection started"));
#endif

  generateSequence();
}

void loop() {
  if ((millis() - lastDebounceTime) > debounceDelay) {
    for (size_t i = 0; i < numInputs; i++) {
      int currentInputState = digitalRead(inputsPins[i]);

      if (currentInputState != lastInputState[i]) {
        lastDebounceTime = millis();
      }

      if (currentInputState == LOW && lastInputState[i] == HIGH) {
        if (steps[currentStep] == i) {
          currentStep++;

#ifdef DEBUG
          Serial.print(F("Correct input! Onto step #"));
          Serial.println(currentStep);
#endif
        } else {
          currentStep = 0;
          Serial.println(F("Incorrect input. Back to the beginning"));
        }
      }

      lastInputState[i] = currentInputState;

      // Turn on the number of leds corrensponding to the current step
      for (int i = 0; i < numSteps; i++) {
        digitalWrite(ledPins[i], (i < currentStep ? HIGH : LOW));
      }
    }
  }

  if (currentStep == numSteps) {
    onSolve();
  }
}