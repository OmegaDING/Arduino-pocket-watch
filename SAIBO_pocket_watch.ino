//源码分四部分，使用IO1按键切换,使用外部中断检测。
//MODE 0 是眼珠子 MODE 1 数字表盘  //MODE 2 圆圈进度表盘  //MODE 4 wifi配网界面
//TFF_eSPI的显示配置引脚见原理图

#include <SPI.h>
#include <TFT_eSPI.h>
#include "U8g2_for_TFT_eSPI.h"
#include "fonts.h"
#include "RX8025.h"
#include "EEPROM.h"
#include "WiFi.h"
#include <esp_wifi.h>
#include "time.h"
#include "esp_adc_cal.h"

int BAT_PIN = 0;
uint16_t BAT_MIN = 2480;
uint16_t BAT_MAX = 3350;
RX8025 rtc;

TFT_eSPI tft;
TFT_eSprite img = TFT_eSprite(&tft);
U8g2_for_TFT_eSPI u8f;

//EEPROM 储存开盖默认显示模式
#define EEPROM_SIZE 64  
int addr = 0;

// A pixel buffer is used during eye rendering
#define BUFFER_SIZE 128 // 128 to 1024 seems optimum


#define BUFFERS 1      // 1 buffer for no DMA

uint8_t MODE;

uint16_t pbuffer[BUFFERS][BUFFER_SIZE]; // Pixel rendering buffer
bool     dmaBuf   = 0;                  // DMA buffer selection

uint16_t pbufferx2[256];
// This struct is populated in config.h
typedef struct {        // Struct is defined before including config.h --
  int8_t  select;       // pin numbers for each eye's screen select line
  int8_t  wink;         // and wink button (or -1 if none) specified there,
  uint8_t rotation;     // also display rotation and the x offset
  int16_t xposition;    // position of eye on the screen
} eyeInfo_t;

#include "config.h"     // ****** CONFIGURATION IS DONE IN HERE ******

#define SCREEN_X_START 0
#define SCREEN_X_END   SCREEN_WIDTH   // Badly named, actually the "eye" width!
#define SCREEN_Y_START 0
#define SCREEN_Y_END   SCREEN_HEIGHT  // Actually "eye" height

// A simple state machine is used to control eye blinks/winks:
#define NOBLINK 0       // Not currently engaged in a blink
#define ENBLINK 1       // Eyelid is currently closing
#define DEBLINK 2       // Eyelid is currently opening
typedef struct {
  uint8_t  state;       // NOBLINK/ENBLINK/DEBLINK
  uint32_t duration;    // Duration of blink state (micros)
  uint32_t startTime;   // Time (micros) of last state change
} eyeBlink;

struct {                // One-per-eye structure
  int16_t   tft_cs;     // Chip select pin for each display
  eyeBlink  blink;      // Current blink/wink state
  int16_t   xposition;  // x position of eye image
} eye[NUM_EYES];

uint32_t startTime;  // For FPS indicator

int lastAngle = 0;
float circle = 100;
bool dir = 0;
int rAngle = 359;

uint8_t OP = 0;
uint8_t DL = 0;
bool BAT_V = 1;
hw_timer_t *timer = NULL;
hw_timer_t *timer1 = NULL;

void IRAM_ATTR BAT_ADC_TimerEvent()
{
  BAT_V = 1;
}

void IRAM_ATTR TimerEvent()
{

  if (OP > 40) {
    timerAlarmDisable(timer);

  } else {
    sigmaDeltaWrite(0, OP++);
  }

}


//外部中断检测按键

const byte interruptPin_0 = 1;  //设置中断引脚
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR handleInterrupt() {//中断服务函数
  portENTER_CRITICAL_ISR(&mux);

  MODE++;
  if (MODE > 3)MODE = 0;
  EEPROM.write(addr, MODE);
  EEPROM.commit();
  ESP.restart();
  portEXIT_CRITICAL_ISR(&mux);
}


//ntp网络对时
const char* ntpServer1 = "ntp3.aliyun.com";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600 * 8;
const int   daylightOffset_sec = 0;

//ADC电池电源检测
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#define ADC_EXAMPLE_ATTEN           ADC_ATTEN_DB_11
static esp_adc_cal_characteristics_t adc1_chars;
bool cali_enable;
int BAT_vol;


uint8_t Month_d[12] = {31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


void setup(void) {

  Serial.begin(115200);
  Serial.println("Starting");
  cali_enable = adc_calibration_init();

  //初始化屏PWM控制幕亮度为关闭
  sigmaDeltaSetup(BLK, 0, 5000);
  sigmaDeltaWrite(0, 0);

  //初始屏幕开机亮度渐亮定时器
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &TimerEvent, true);
  timerAlarmWrite(timer, 20000, true);

  //初始化ADC定时器
  timer1 = timerBegin(1, 80, true);
  timerAttachInterrupt(timer1, &BAT_ADC_TimerEvent, true);
  timerAlarmWrite(timer1, 3000000, true);
  timerAlarmEnable(timer1);

  //读取模式
  EEPROM.begin(EEPROM_SIZE);
  MODE = EEPROM.read(addr);
  Serial.print("MODE:");
  Serial.println(MODE);
  pinMode(interruptPin_0, INPUT_PULLUP);
  //初始外部中断
  attachInterrupt(digitalPinToInterrupt(interruptPin_0), handleInterrupt, FALLING);
  //初始化I2C
  Wire.begin(4, 5);
  //初始RX8025
  rtc.RX8025_init();
  rtc.getYear();
  Serial.println(rtc.getYear());
  if(rtc.getYear()<20){
    rtc.setRtcTime(0, 0, 0, 0 , 1, 3, 23);
    rtc.setRtcTime(0, 0, 0, 0 , 1, 3, 23);
  }

  if (BAT_Votage() < BAT_MIN)MODE = 99;

  //判断模式
  if (MODE == 0) { //start eye
    // Initialise the eye(s), this will set all chip selects low for the tft.init()
    tft.init();
    tft.setRotation(1);
    initEyes();

    // Initialise TFT
    Serial.println("Initialising displays");

    startTime = millis(); // For frame-rate calculation
  } else if (MODE == 1) { //start watch


    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);

    img.setSwapBytes(true);
    img.createSprite(240, 240);
    img.setTextDatum(4);

    u8f.begin(img);
    u8f.setFontMode(0);                 // use u8g2 none transparent mode
    u8f.setFontDirection(0);            // left to right (this is default)
    u8f.setForegroundColor(TFT_WHITE);  // apply colors
    u8f.setFont(u8g2_font_wqy12_t_gb2312);


    int b = 0;
    int b2 = 0;


    for (int i = 0; i < 360; i++)
    {
      x[i] = (r * cos(rad * i)) + sx;
      y[i] = (r * sin(rad * i)) + sy;
      px[i] = ((r - 16) * cos(rad * i)) + sx;
      py[i] = ((r - 16) * sin(rad * i)) + sy;

      lx[i] = ((r - 26) * cos(rad * i)) + sx;
      ly[i] = ((r - 26) * sin(rad * i)) + sy;

      if (i % 30 == 0) {
        start[b] = i;
        b++;
      }

      if (i % 6 == 0) {
        startP[b2] = i;
        b2++;
      }
    }
  }
  else if (MODE == 2) { //Progress bar
    tft.init();
    tft.setRotation(1);
    tft.setSwapBytes(true);
    tft.fillScreen(TFT_BLACK);


    img.setSwapBytes(true);
    img.createSprite(240, 240);
    img.setTextDatum(4);

    u8f.begin(img);
    u8f.setFontMode(0);                 // use u8g2 none transparent mode
    u8f.setFontDirection(0);            // left to right (this is default)
    u8f.setForegroundColor(TFT_WHITE);  // apply colors
    u8f.setFont(u8g2_font_wqy12_t_gb2312);
  }
  else if (MODE == 3) {


    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    u8f.begin(tft);
    u8f.setFontMode(0);                 // use u8g2 none transparent mode
    u8f.setFontDirection(0);            // left to right (this is default)
    u8f.setForegroundColor(TFT_WHITE);  // apply colors
    u8f.setBackgroundColor(TFT_BLACK);
    u8f.setFont(u8g2_font_wqy12_t_gb2312);
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  }

  timerAlarmEnable(timer);  //  使能定时器

  if (MODE == 2) {
    Progressinit();
  }

  if (MODE == 3) {
    wifi_init();
  }
  if (MODE == 99) {
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextSize(2);
    rtc.getHour();
    rtc.getMinute();
    rtc.getSecond();

    tft.setCursor(50, 90);

    tft.printf("%02d : %02d : %02d", rtc.getHour(), rtc.getMinute(), rtc.getSecond());

    u8f.begin(tft);
    u8f.setFontMode(0);                 // use u8g2 none transparent mode
    u8f.setFontDirection(0);            // left to right (this is default)
    u8f.setForegroundColor(TFT_WHITE);  // apply colors
    u8f.setFont(u8g2_font_wqy12_t_gb2312);



    u8f.setCursor(80, 130);
    u8f.print("电池电量过低！");

    delay(2000);
    esp_deep_sleep_start();
  }
}

// MAIN LOOP

void loop() {
  if (MODE == 0) {
    updateEye();
  } else if (MODE == 1) {
    updateWatch();
  } else if (MODE == 2) {
    Progress();
  } else if (MODE == 3) {
    printLocalTime();
    delay(1000);
  }

}
