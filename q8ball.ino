#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LASER_PIN 2
#define PHOTO_RESISTOR_A0 A0
#define PHOTO_RESISTOR_A1 A1
#define DURATION 2000 // Duration for laser and sensor readings in milliseconds
#define CALIBRATION_TIME 2000 // Calibration time in milliseconds

const char* positiveResponses[] = {
  "Yes",
  "Most likely",
  "Yes, definitely",
  "It is decidedly so",
  "Without a doubt",
  "As I see it, yes",
  "Outlook good",
  "Yes, in due time",
  "Definitely",
  "You may rely on it"
};

const char* negativeResponses[] = {
  "Don't count on it",
  "My sources say no",
  "Outlook not so good",
  "Very doubtful",
  "Impossible"
};

const char* nonCommittalResponses[] = {
  "Ask again later",
  "Cannot predict now",
  "Concentrate and ask again",
  "Too early to tell",
  "No answer right now"
};

int baselineA0 = 0, baselineA1 = 0;
bool cursorVisible = false;  // For blinking cursor

void setup() {
  pinMode(LASER_PIN, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
  display.display();
  randomSeed(analogRead(0));
}

void loop() {
  calibrateSensors();
  charging();
  readyToFire();
  systemStandby();
}

void calibrateSensors() {
  displayMessage("CALIBRATING..", -1, -1, false);
  
  int totalReadingsA0 = 0, totalReadingsA1 = 0;
  int numReadings = 0;

  unsigned long calibrationStartTime = millis();
  while (millis() - calibrationStartTime < CALIBRATION_TIME) {
    int ambientA0 = analogRead(PHOTO_RESISTOR_A0);
    int ambientA1 = analogRead(PHOTO_RESISTOR_A1);

    totalReadingsA0 += ambientA0;
    totalReadingsA1 += ambientA1;
    numReadings++;
    
    displayMessage("CALIBRATING..", ambientA0, ambientA1, false);
    delay(10);
  }

  baselineA0 = totalReadingsA0 / numReadings;
  baselineA1 = totalReadingsA1 / numReadings;

  int adjustmentFactor = abs(baselineA0 - baselineA1);  // Calculate the adjustment factor
  
  displayMessage("CALIBRATED", adjustmentFactor, -1, true);
  delay(2000);
}

void charging() {
  const char spinner[] = {'|', '/', '-', '\\'};
  const int max_width = 15; // Set max width for animation
  for (int i = 0; i < max_width; i++) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("CHARGING BEAM");

    display.setCursor(0, 16);
    for (int j = 0; j < i; j++) {
      display.print("-");
    }
    display.print(spinner[i % 4]);
    display.display();
    delay(50);
  }
}

void readyToFire() {
  digitalWrite(LASER_PIN, HIGH);
  unsigned long startTime = millis();
  
  while (millis() - startTime < DURATION) {
    int correctedA0 = analogRead(PHOTO_RESISTOR_A0) - baselineA0;
    int correctedA1 = analogRead(PHOTO_RESISTOR_A1) - baselineA1;

    displayMessage("SPLITTING PHOTONS..", correctedA0, correctedA1, false); 
    delay(10);
  }
  
  digitalWrite(LASER_PIN, LOW);

  const char* response;
  int correctedA0 = analogRead(PHOTO_RESISTOR_A0) - baselineA0;
  int correctedA1 = analogRead(PHOTO_RESISTOR_A1) - baselineA1;
  if (correctedA0 > correctedA1) {
    int index = random(0, 5); 
    response = negativeResponses[index];
  } 
  else if (correctedA1 > correctedA0) {
    int index = random(0, 10); 
    response = positiveResponses[index];
  } 
  else {
    int index = random(0, 5); 
    response = nonCommittalResponses[index];
  }

  displayMessage(response, correctedA0, correctedA1, false); 
  delay(4000);
}

void systemStandby() {
  while (true) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("SYS STANDING BY");
    if (cursorVisible) {
      display.print("_");
    }
    display.display();
    cursorVisible = !cursorVisible; // Toggle cursor visibility
    delay(500);
  }
}

void displayMessage(const char* message, int valA0, int valA1, bool showOffset) {
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0); 
  display.print(message);

  if (showOffset && valA0 != -1) {
    display.setCursor(0, 16);
    display.print("ADJ.FACTOR: +/- ");
    display.println(valA0);
  }

  if (!showOffset && valA0 != -1 && valA1 != -1) {
    display.setCursor(0, 16);
    display.print("R-A0 (E/W-POL): ");
    display.println(valA0);
    display.print("R-A1 (N/S-POL): ");
    display.println(valA1);
  }

  display.display();
}
