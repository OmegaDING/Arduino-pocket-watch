// Pin selections here are based on the original Adafruit Learning System
// guide for the Teensy 3.x project.  Some of these pin numbers don't even
// exist on the smaller SAMD M0 & M4 boards, so you may need to make other
// selections:

// GRAPHICS SETTINGS (appearance of eye) -----------------------------------

// If using a SINGLE EYE, you might want this next line enabled, which
// uses a simpler "football-shaped" eye that's left/right symmetrical.
// Default shape includes the caruncle, creating distinct left/right eyes.

//#define SYMMETRICAL_EYELID

// Enable ONE of these #includes -- HUGE graphics tables for various eyes:
#include "data/defaultEye.h"      // Standard human-ish hazel eye -OR-
//#include "data/dragonEye.h"     // Slit pupil fiery dragon/demon eye -OR-
//#include "data/noScleraEye.h"   // Large iris, no sclera -OR-
//#include "data/goatEye.h"       // Horizontal pupil goat/Krampus eye -OR-
//#include "data/newtEye.h"       // Eye of newt -OR-
//#include "data/terminatorEye.h" // Git to da choppah!
//#include "data/catEye.h"        // Cartoonish cat (flat "2D" colors)
//#include "data/owlEye.h"        // Minerva the owl (DISABLE TRACKING)
//#include "data/naugaEye.h"      // Nauga googly eye (DISABLE TRACKING)
//#include "data/doeEye.h"        // Cartoon deer eye (DISABLE TRACKING)

// DISPLAY HARDWARE SETTINGS (screen type & connections) -------------------
#define TFT_COUNT 1        // Number of screens (1 or 2)
#define TFT1_CS -1         // TFT 1 chip select pin (set to -1 to use TFT_eSPI setup)
#define TFT_1_ROT 0        // TFT 1 rotation
#define EYE_1_XPOSITION  0         // x shift for eye 1 image on display

#define BLK 7


// EYE LIST ----------------------------------------------------------------
#define NUM_EYES 1 // Number of eyes to display (1 or 2)

#define BLINK_PIN   -1 // Pin for manual blink button (BOTH eyes)
#define LH_WINK_PIN -1 // Left wink pin (set to -1 for no pin)


  eyeInfo_t eyeInfo[] = {
    { TFT1_CS, LH_WINK_PIN, TFT_1_ROT, EYE_1_XPOSITION }, // EYE chip select and wink pins, rotation and offset
  };



#define TRACKING            // If defined, eyelid tracks pupil
#define AUTOBLINK           // If defined, eyes also blink autonomously

//  #define LIGHT_PIN      -1 // Light sensor pin
  #define LIGHT_CURVE  0.33 // Light sensor adjustment curve
  #define LIGHT_MIN       0 // Minimum useful reading from light sensor
  #define LIGHT_MAX    1023 // Maximum useful reading from sensor

#define IRIS_SMOOTH         // If enabled, filter input from IRIS_PIN
#if !defined(IRIS_MIN)      // Each eye might have its own MIN/MAX
  #define IRIS_MIN       90 // Iris size (0-1023) in brightest light
#endif
#if !defined(IRIS_MAX)
  #define IRIS_MAX      130 // Iris size (0-1023) in darkest light
#endif


//watch_coinfg

#define color1 TFT_WHITE
#define color2  0x8410
#define color3 0x5ACB
#define color4 0x15B3
#define color5 0x00A3

volatile int counter = 0;
float VALUE;
float lastValue = 0;

double rad = 0.01745;
float x[360];
float y[360];


float px[360];
float py[360];

float lx[360];
float ly[360];


int r = 104;
int sx = 120;
int sy = 120;

String cc[12] = {"45", "40", "35", "30", "25", "20", "15", "10", "05", "0", "55", "50"};
String days[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
String moondays[] = {"","一","二","三","四","五","六","七","八","九","十",
                     "十一","十二","十三","十四","十五","十六","十七","十八","十九","二十",
                     "廿一","廿二","廿三","廿四","廿五","廿六","廿七","廿八","廿九","三十"};
int start[12];
int startP[60];

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;


int angle = 0;
bool onOff = 0;
bool debounce = 0;

String h, m, s, d1, d2, m1, m2;
