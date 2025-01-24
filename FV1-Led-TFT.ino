#include <TFT.h>
#include <SPI.h>
#include <Encoder.h>
#include <EEPROM.h>

#define DEBUG 1    // ON/OFF switch

#if DEBUG == 1
#define debug(x)   Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// OLED Display Setup
#define tftCS   10
#define tftRST   9
#define tftDC    8
TFT tft = TFT(tftCS, tftDC, tftRST);

// Encoder Pins
#define pinEncoderCW   2
#define pinEncoderCCW  3

// Encoder Setup
Encoder myEnc(pinEncoderCW, pinEncoderCCW);

struct Item {
  String name;
  int params[3];
};
Item items[24];

int currentItem   = 0; // Last selected effect index
int currentParam  = 0;
bool editingMode  = false;
unsigned long lastInteractionTime = 0;

// EEPROM Addresses
#define EEPROM_SIGNATURE_ADDR    0
#define EEPROM_LAST_EFFECT_ADDR  1
#define EEPROM_PARAMS_START_ADDR 2
#define EEPROM_SIGNATURE      0xA5


//
void setup() {
  // Serial for debugging
  Serial.begin(9600);

  // Initialize OLED
  tft.begin();
  tft.setRotation(1);
  tft.background(0,0,0);

  // Configure button and encoder
  //pinMode(BUTTON_PIN, INPUT_PULLUP);  // Button with pull-up
  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonInterrupt, FALLING);

  // Initialize Items with the requested names
  String names[24] = {
    "CHORUS.0", "REVERB.a0", "REVERB.b0", "Tremo.a0", "Tremo.b0", "Flanger.0", "PITCH\n   Octave.0", "DISTORTION\n   v0",
    "CHORUS.1", "REVERB.a1", "REVERB.b1", "Tremo.a1", "Tremo.b1", "Flanger.1", "Pitch\n   Octave.1", "Distort.1",
    "CHORUS.2", "REVERB.a2", "REVERB.b2", "Tremo.a2", "Tremo.b2", "Flanger.2", "Pitch\n   Octave.2", "Distort.2"
  };

  for (int i = 0; i < 24; i++) {
    items[i].name = names[i];
  }

  // Check EEPROM Signature
  if (EEPROM.read(EEPROM_SIGNATURE_ADDR) != EEPROM_SIGNATURE) {
    // Initialize EEPROM with default values if signature is missing
    for (int i = 0; i < 24; i++) {
      for (int j = 0; j < 3; j++) {
        items[i].params[j] = 100; // Default parameters
      }
    }
    currentItem = 0;            // Default to first effect
    saveParameters();           // Save all parameters to EEPROM
    saveLastSelectedEffect();   // Save initial selected effect
    EEPROM.write(EEPROM_SIGNATURE_ADDR, EEPROM_SIGNATURE); // Write signature
  } else {
    // Load all parameters and last selected effect from EEPROM
    loadParameters();
    currentItem = loadLastSelectedEffect();
  }
  // Draw the initial screen
  drawItem();

  //tft.textSize(2);
  //tft.stroke(128,128,128); // LIGHT BLUE
  //tft.text("Hey!!!",20,20);
 
/*
  tft.text("Hello", tft.width()/2, 20);

  tft.stroke(255,0,255);
  tft.circle(tft.width()/2, 64, 22);

  tft.fill(0,0,255);  // RED 
  tft.stroke(255,0,0); // BLUE
  //tft.noStroke();      // No Frame

  tft.rect(50,50,30,30);
  */
}


// LOOP
void loop() {
  static long lastPosition = -999;
  long newPosition = myEnc.read() / 2; // Adjust for sensitivity
 
  // Handle encoder rotation for item selection
  if (newPosition != lastPosition) {
    currentItem = (currentItem + (newPosition > lastPosition ? 1 : -1) + 24) % 24;
    lastPosition = newPosition;                     // Update the last position
    drawItem();
    lastInteractionTime = millis();
  }
}


void drawFrame() {
  tft.stroke(0,255,255); // 255,0,0 - BLUE | 0,255,255 - YELLOW
  tft.rect(0,0,160,128);
  tft.rect(1,1,158,126);
  tft.rect(2,2,156,124);
}

//
void drawItem() {
  tft.fill(0,0,0); // Clear screen to BLACK
  drawFrame();
  
  // Centered Title (Effect Number and Name)
  tft.stroke(255,0,0); //tft.setTextColor(ST77XX_BLUE);
  tft.textSize(2);     //tft.setTextSize(2);

  String effectText = String(currentItem + 1) + "." + items[currentItem].name;
  int textX = 10; 
  tft.text(effectText.c_str(), textX, 12);  //tft.setCursor(textX, 12);
  // Parameters Display with Horizontal Bars
  tft.textSize(1);
  for (int i = 0; i < 3; i++) {
    //
    if (editingMode && i == currentParam) {
       tft.stroke(0,0,255); //color = tft.color565(0, 0, 255); // Highlighted blue
    }
    int barY = 60 + i * 16;
    // PARAM
    tft.stroke(255,255,255); //tft.setTextColor(ST77XX_WHITE);
    tft.text("0", 10, barY); //tft.print("0"); // Left of bar

    //tft.stroke(255,255,0);  //tft.fillRect(20, barY, items[currentItem].params[i], 10, color); // Horizontal blue bar
    tft.rect(20, barY, 100, 10);//tft.drawRect(20, barY, 100, 10, ST77XX_WHITE);
    // NAME
    //tft.noStroke();
    String paramLevel = String(items[currentItem].params[i]);
    tft.text(paramLevel.c_str(), 126, barY);
    tft.text("%", 140, barY); //tft.print(items[currentItem].params[i]);
  }
}

void saveParameters() {
  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 3; j++) {
      int addr = EEPROM_PARAMS_START_ADDR + i * 3 + j; // Calculate address
      EEPROM.update(addr, items[i].params[j]); // Save only if value changes
    }
  }
}

void loadParameters() {
  for (int i = 0; i < 24; i++) {
    for (int j = 0; j < 3; j++) {
      int addr = EEPROM_PARAMS_START_ADDR + i * 3 + j; // Calculate address
      items[i].params[j] = EEPROM.read(addr); // Load saved parameter
    }
  }
}

void saveLastSelectedEffect() {
  EEPROM.update(EEPROM_LAST_EFFECT_ADDR, currentItem);
}

int loadLastSelectedEffect() {
  return EEPROM.read(EEPROM_LAST_EFFECT_ADDR);
}
