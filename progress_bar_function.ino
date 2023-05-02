void Progressinit() {
  daycount();
  uint16_t Pd = map(daycount(), 0, 365, 0, 360);

  uint16_t Md = map(rtc.getDate(), 0, Month_d[rtc.getMonth() - 1], 0, 360);
  uint32_t Sd = (rtc.getHour() * 3600) + (rtc.getMinute() * 60 ) + rtc.getSecond();
  uint16_t Hd = map(Sd, 0, 86400 , 0, 360);


  uint16_t x, y;
  for (int i = 0; i < 360; i ++) {
    x = (110 * cos(rad * (270 + i))) + 120;
    y = (110 * sin(rad * (270 + i))) + 120;
    tft.fillCircle(x, y, 8, 0x0006);
    x = (90 * cos(rad * (270 + i))) + 120;
    y = (90 * sin(rad * (270 + i))) + 120;
    tft.fillCircle(x, y, 8, 0x180);
    x = (70 * cos(rad * (270 + i))) + 120;
    y = (70 * sin(rad * (270 + i))) + 120;
    tft.fillCircle(x, y, 8, 0x3040);
  }
  for (int i = 0; i < 360; i ++) {
    if (i < Pd) {
      x = (110 * cos(rad * (270 + i))) + 120;
      y = (110 * sin(rad * (270 + i))) + 120;
      tft.fillCircle(x, y, 8, TFT_BLUE);
    }
    if (i < Md) {
      x = (90 * cos(rad * (270 + i))) + 120;
      y = (90 * sin(rad * (270 + i))) + 120;
      tft.fillCircle(x, y, 8, TFT_GREEN);
    }
    if (i < Hd) {
      x = (70 * cos(rad * (270 + i))) + 120;
      y = (70 * sin(rad * (270 + i))) + 120;
      tft.fillCircle(x, y, 8, TFT_ORANGE);
    }
    delay(2);
  }
}

void Progress() {
  uint16_t Pd = map(daycount(), 0, 365, 0, 360);
  uint16_t Md = map(rtc.getDate(), 0, Month_d[rtc.getMonth() - 1], 0, 360);
  uint8_t HH = rtc.getHour();
  uint8_t MM = rtc.getMinute();
  uint8_t SS = rtc.getSecond();

  uint32_t Sd = (HH * 3600) + (MM * 60 ) + SS;
  uint16_t Hd = map(Sd, 0, 86400 , 0, 360);

  drawcount(Pd, Md, Hd, HH, MM, SS);

}



uint16_t daycount() {

  int year, month, days , Pd = 0;

  year = rtc.getYear() + 2000;
  month = rtc.getMonth();
  days = rtc.getDate();


  if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) { //闰年的判断
    Month_d[1] = 29;
  }
  else
  {
    Month_d[1] = 28;
  }

  for (int i = 0 ; i < month - 1; i ++) {

    Pd = Pd + Month_d[i];
  }
  Pd = Pd + days;

  return Pd;

}

void drawcount(uint16_t pd, uint16_t md, uint16_t hd , uint8_t hh, uint8_t mm, uint8_t ss) {
  uint16_t x, y;
  img.fillSprite(TFT_BLACK);
  for (int i = 0; i < 360; i ++) {
    x = (110 * cos(rad * (270 + i))) + 120;
    y = (110 * sin(rad * (270 + i))) + 120;
    img.fillCircle(x, y, 8, 0x0006);
    x = (90 * cos(rad * (270 + i))) + 120;
    y = (90 * sin(rad * (270 + i))) + 120;
    img.fillCircle(x, y, 8, 0x180);
    x = (70 * cos(rad * (270 + i))) + 120;
    y = (70 * sin(rad * (270 + i))) + 120;
    img.fillCircle(x, y, 8, 0x3040);
  }
  for (int i = 0; i < 360; i ++) {
    if (i < pd) {
      x = (110 * cos(rad * (270 + i))) + 120;
      y = (110 * sin(rad * (270 + i))) + 120;
      img.fillCircle(x, y, 8, TFT_BLUE);
    }
    if (i < md) {
      x = (90 * cos(rad * (270 + i))) + 120;
      y = (90 * sin(rad * (270 + i))) + 120;
      img.fillCircle(x, y, 8, TFT_GREEN);
    }
    if (i < hd) {
      x = (70 * cos(rad * (270 + i))) + 120;
      y = (70 * sin(rad * (270 + i))) + 120;
      img.fillCircle(x, y, 8, TFT_ORANGE);
    }
  }

  uint16_t Sd = map(ss, 0, 59, 0, 359);
  x = (50 * cos(rad * (270 + Sd))) + 120;
  y = (50 * sin(rad * (270 + Sd))) + 120;
  img.fillCircle(x, y, 4, TFT_RED);

  img.setFreeFont(&DSEG7_Classic_Bold_17);
  img.setTextColor(TFT_GREEN, TFT_BLACK);
  img.setCursor(90, 115);
  img.printf("%02d:%02d", hh, mm);
  img.setFreeFont(&DSEG7_Classic_Bold_10);
  img.setCursor(77, 130);
  img.printf("%d-%02d-%02d", rtc.getYear() + 2000, rtc.getMonth(), rtc.getDate());

  if (BAT_V) {
    int B = BAT_Votage();
    if(B>BAT_MAX)B = BAT_MAX;
    BAT_vol = map(B, 2600, BAT_MAX, 0, 100);
    if (B < BAT_MIN) {
      img.fillScreen(TFT_BLACK);
      img.setTextColor(TFT_GREEN, TFT_BLACK);
      img.setTextFont(2);
      img.setTextSize(2);
      img.setCursor(50, 90);
      img.printf("%02d : %02d : %02d", rtc.getHour(), rtc.getMinute(), rtc.getSecond());
      u8f.setCursor(80, 130);
      u8f.print("电池电量过低！");
      img.pushSprite(0, 0);

      delay(2000);
      esp_deep_sleep_start();
    }
    BAT_V = 0;
  }
  img.fillRoundRect(108, 140, 28, 8, 3, TFT_GREEN);
  img.fillRoundRect(109, 141, 26, 6, 3, TFT_BLACK);
  uint8_t b = map(BAT_vol,100,0,0,24);
  img.fillRoundRect(110+b, 142, 24-b, 4, 3, TFT_ORANGE);
  
  img.pushSprite(0, 0);
}
