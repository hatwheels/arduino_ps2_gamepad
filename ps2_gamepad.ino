/***********************************************************************************************************************
 *  Intro: PS2 GamePad
 *         Arduino reads and prints all data and commands of the PS2 gamepad.
 *         Analog Sticks are filtered using Radial Scaling method and
 *         printed when there is a change.
 *         Uses PS2X library for Arduino (https://github.com/madsci1016/Arduino-PS2X).
 *
 *  Board: Arduino Pro Mini ATmega 328 (3.3V, 8MHz)
 *
 *  Connections:  Type    Pro Mini    PS2 controller      Components
 *                Output  D2          Data (brown)        R1
 *                Output  D3          Attention (green)
 *                Output  D4          Command (orange)
 *                Output  D5          Clock (blue)
 *                Output  VCC         3.3V DC             R1
 *                Output  GND         GND
 *
 **********************************************************************************************************************/

#include <PS2X_lib.h>

//Set pins connected to PS2 controller:
#define PS2_DAT        2    
#define PS2_CMD        4
#define PS2_SEL        3
#define PS2_CLK        5

//Select modes of PS2 controller:
// - pressures = analog reading of push-butttons 
// - rumble    = motor rumbling
#define pressures false
#define rumble    false

//Radial Scaling Constants
const int rMax = sqrt(2 * pow(128, 2));
const int deadzone = 0.10 * rMax;
const int rScaledMax = rMax / (rMax - deadzone);

//Create PS2 Controller Class
PS2X ps2x; 

int error = 0;
byte type = 0;
byte vibrate = 0;

/**********************************************************************************************************************
 *  Static Functions
 *********************************************************************************************************************/
static int analogStickTranspose(int val, bool invert) {
  if (val > 128) {
    return ((invert) ? (127 - val) : (val - 127));
  } else if (val < 127) {
    return ((invert) ? (128 - val) : (val - 128));
  } else {
    return 0;
  }
}

static void analogStickRadialScaling(int *xCurr, int *yCurr) {
  int r = sqrt(pow(*xCurr, 2) + pow(*yCurr, 2));
  
  if (r < deadzone) {
    *xCurr = 0;
    *yCurr = 0;
  } else {
    int rScaled = r / (r - deadzone);
    
    *xCurr = rScaledMax / rScaled * *xCurr;
    *yCurr = rScaledMax / rScaled * *yCurr;
  }  
}

static void analogStickPrint(int *xPrev, int *yPrev, int xCurr, int yCurr, char *str) {
  if (*xPrev != xCurr || *yPrev != yCurr) {
    if (*xPrev != xCurr) {
      *xPrev = xCurr;
    }
    if (*yPrev != yCurr) {
      *yPrev = yCurr;
    }
    Serial.print(str);
    Serial.print(F("Stick: ("));
    Serial.print(*xPrev);
    Serial.print(F(","));
    Serial.print(*yPrev);
    Serial.println(F(")"));
  }  
}

static void analogStick(void) {
  static int lxPrev = -128;
  static int lyPrev = 128;
  static int rxPrev = -128;
  static int ryPrev = 128;
  
  int lxCurr = analogStickTranspose(ps2x.Analog(PSS_LX), false);
  int lyCurr = analogStickTranspose(ps2x.Analog(PSS_LY), true);
  int rxCurr = analogStickTranspose(ps2x.Analog(PSS_RX), false);
  int ryCurr = analogStickTranspose(ps2x.Analog(PSS_RY), true);
  
  analogStickRadialScaling(&lxCurr, &lyCurr);
  analogStickPrint(&lxPrev, &lyPrev, lxCurr, lyCurr, "Left ");
  analogStickRadialScaling(&rxCurr, &ryCurr);
  analogStickPrint(&rxPrev, &ryPrev, rxCurr, ryCurr, "Right ");
}

/**********************************************************************************************************************
 *  Setup
 *********************************************************************************************************************/
void setup() {
  Serial.begin(57600); 
  delay(300);

  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error  
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if (error == 0) {
    Serial.print(F("Found Controller, configured successful "));
    Serial.print(F("pressures = "));
    if (pressures) {
      Serial.println(F("true "));
    } else {
      Serial.println(F("false"));
    }
    Serial.print(F("rumble = "));
    if (rumble) {
      Serial.println(F("true)"));
    } else {
      Serial.println(F("false"));
    }
    Serial.println(F("Try out all the buttons, X will vibrate the controller, faster as you press harder;"));
    Serial.println(F("holding L1 or R1 will print out the analog stick values."));
    Serial.println(F("Note: Go to www.billporter.info for updates and to report bugs."));
  } else if (error == 1) {
    Serial.println(F("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips"));
  } else if (error == 2) {
    Serial.println(F("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips"));
  } else if (error == 3) {
    Serial.println(F("Controller refusing to enter Pressures mode, may not support it. "));
  }
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.println(F("Unknown Controller type found "));
      break;
    case 1:
      Serial.println(F("DualShock Controller found "));
      break;
    case 2:
      Serial.println(F("GuitarHero Controller found "));
      break;
  case 3:
      Serial.println(F("Wireless Sony DualShock Controller found "));
      break;
   }
   Serial.println();
}

/**********************************************************************************************************************
 *  Main
 *********************************************************************************************************************/
void loop() {
  /* You must Read Gamepad to get new values and set vibration values:
      ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     If you don't enable the rumble, use:
      ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */
   
  if (error == 1) //Skip loop if no controller found
    return;
    
  if (type == 2) { //Guitar Hero Controller
    ps2x.read_gamepad();
    
    if (ps2x.ButtonPressed(GREEN_FRET))
      Serial.println(F("Green Fret Pressed"));
    if (ps2x.ButtonPressed(RED_FRET))
      Serial.println(F("Red Fret Pressed"));
    if (ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println(F("Yellow Fret Pressed"));
    if (ps2x.ButtonPressed(BLUE_FRET))
      Serial.println(F("Blue Fret Pressed"));
    if (ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println(F("Orange Fret Pressed")); 

    if (ps2x.ButtonPressed(STAR_POWER))
      Serial.println(F("Star Power Command"));
    
    if (ps2x.Button(UP_STRUM)) //will be TRUE as long as button is pressed
      Serial.println(F("Up Strum"));
    if (ps2x.Button(DOWN_STRUM))
      Serial.println(F("DOWN Strum"));
 
    if (ps2x.Button(PSB_START)) //will be TRUE as long as button is pressed
      Serial.println(F("Start is being held"));
    if (ps2x.Button(PSB_SELECT))
      Serial.println(F("Select is being held"));
    
    if (ps2x.Button(ORANGE_FRET)) { //Print stick value IF TRUE
      Serial.print(F("Wammy Bar Position:"));
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC); 
    } 
  } else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    // 'START' button
    if (ps2x.ButtonPressed(PSB_START)) { //will be TRUE if button was JUST pressed
      Serial.println(F("Start pressed"));
    } else if (ps2x.ButtonReleased(PSB_START)) { //will be TRUE if button was JUST pressed
      Serial.println(F("Start released"));
    }
    // 'SELECT' button
    if (ps2x.ButtonPressed(PSB_SELECT)) { //will be TRUE if button was JUST pressed
      Serial.println(F("Select pressed"));
    } else if (ps2x.ButtonReleased(PSB_SELECT)) { //will be TRUE if button was JUST pressed
      Serial.println(F("Select released"));
    }
    // 'D-PAD UP' button
    if (ps2x.ButtonPressed(PSB_PAD_UP)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Up pressed"));
    } else if (ps2x.ButtonReleased(PSB_PAD_UP)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Up released"));
    }
    // 'D-PAD DOWN' button
    if (ps2x.ButtonPressed(PSB_PAD_DOWN)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Down pressed"));
    } else if (ps2x.ButtonReleased(PSB_PAD_DOWN)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Down released"));
    }
    // 'D-PAD LEFT' button
    if (ps2x.ButtonPressed(PSB_PAD_LEFT)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Left pressed"));
    } else if (ps2x.ButtonReleased(PSB_PAD_LEFT)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Left released"));
    }
    // 'D-PAD RIGHT' button
    if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Right pressed"));
    } else if (ps2x.ButtonReleased(PSB_PAD_RIGHT)) { //will be TRUE if button was JUST pressed
      Serial.println(F("D-Pad Right released"));
    }
    // 'X' button
    if (ps2x.ButtonPressed(PSB_CROSS)) { //will be TRUE if button was JUST pressed
      Serial.println(F("X pressed"));
    } else if (ps2x.ButtonReleased(PSB_CROSS)) { //will be TRUE if button was JUST pressed
      Serial.println(F("X released"));
    }
    // 'CIRCLE' button
    if (ps2x.ButtonPressed(PSB_CIRCLE)) { //will be TRUE if button was JUST pressed
      Serial.println(F("CIRCLE pressed"));
    } else if (ps2x.ButtonReleased(PSB_CIRCLE)) { //will be TRUE if button was JUST pressed
      Serial.println(F("CIRCLE released"));
    }
    // 'SQUARE' button
    if (ps2x.ButtonPressed(PSB_SQUARE)) { //will be TRUE if button was JUST pressed
      Serial.println(F("SQUARE pressed"));
    } else if (ps2x.ButtonReleased(PSB_SQUARE)) { //will be TRUE if button was JUST pressed
      Serial.println(F("SQUARE released"));
    }
    // 'TRIANGLE' button
    if (ps2x.ButtonPressed(PSB_TRIANGLE)) { //will be TRUE if button was JUST pressed
      Serial.println(F("TRIANGLE pressed"));
    } else if (ps2x.ButtonReleased(PSB_TRIANGLE)) { //will be TRUE if button was JUST pressed
      Serial.println(F("TRIANGLE released"));
    }
    // Buttons L1, L2, R1, R2
    if (ps2x.ButtonPressed(PSB_L1)) { //will be TRUE if button was JUST pressed
      Serial.println(F("L1 pressed"));
    } else if (ps2x.ButtonReleased(PSB_L1)) { //will be TRUE if button was JUST pressed
      Serial.println(F("L1 released"));
    }
    if (ps2x.ButtonPressed(PSB_L2)) { //will be TRUE if button was JUST pressed
      Serial.println(F("L2 pressed"));
    } else if (ps2x.ButtonReleased(PSB_L2)) { //will be TRUE if button was JUST pressed
      Serial.println(F("L2 released"));
    }
    if (ps2x.ButtonPressed(PSB_R1)) { //will be TRUE if button was JUST pressed
      Serial.println(F("R1 pressed"));
    } else if (ps2x.ButtonReleased(PSB_R1)) { //will be TRUE if button was JUST pressed
      Serial.println(F("R1 released"));
    }
    if (ps2x.ButtonPressed(PSB_R2)) { //will be TRUE if button was JUST pressed
      Serial.println(F("R2 pressed"));
    } else if (ps2x.ButtonReleased(PSB_R2)) { //will be TRUE if button was JUST pressed
      Serial.println(F("R2 released"));
    }
    // Analog Stick Buttons L3, R3
    if (ps2x.ButtonPressed(PSB_L3)) { //will be TRUE if button was JUST pressed
      Serial.println(F("L3 pressed"));
    } else if (ps2x.ButtonReleased(PSB_L3)) { //will be TRUE if button was JUST pressed
      Serial.println(F("L3 released"));
    }
    if (ps2x.ButtonPressed(PSB_R3)) { //will be TRUE if button was JUST pressed
      Serial.println(F("R3 pressed"));
    } else if (ps2x.ButtonReleased(PSB_R3)) { //will be TRUE if button was JUST pressed
      Serial.println(F("R3 released"));
    }
    // Left and Right Analog Sticks
    analogStick();
  }
  delay(50);
}
