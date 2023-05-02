void updateWatch() {
  
  rAngle = rAngle - 2;

  angle = rtc.getSecond() * 6;

  s = String(rtc.getSecond());
  m = String(rtc.getMinute());
  h = String(rtc.getHour());
  if (m.toInt() < 10)
    m = "0" + m;

  if (h.toInt() < 10)
    h = "0" + h;

  if (s.toInt() < 10)
    s = "0" + s;


  if (rtc.getDate() > 10)
  {
    d1 = rtc.getDate() / 10;
    d2 = rtc.getDate() % 10;
  }
  else
  {
    d1 = "0";
    d2 = String(rtc.getDate());
  }

  if (rtc.getMonth() > 10)
  {
    m1 = rtc.getMonth() / 10;
    m2 = rtc.getMonth() % 10;
  }
  else
  {
    m1 = "0";
    m2 = String(rtc.getMonth());
  }


  if (angle >= 360)
    angle = 0;

  if (rAngle <= 0)
    rAngle = 359;



  if (dir == 0)
    circle = circle + 0.5;
  else
    circle = circle - 0.5;

  if (circle > 140)
    dir = !dir;

  if (circle < 100)
    dir = !dir;



  if (angle > -1)
  {
    lastAngle = angle;

    VALUE = ((angle - 270) / 3.60) * -1;
    if (VALUE < 0)
      VALUE = VALUE + 100;



    img.fillSprite(TFT_BLACK);
    img.fillCircle(sx, sy, 124, color5);

    img.setTextColor(TFT_WHITE, color5);

    //img.drawString(days[rtc.getDoW()],circle,120,2);
    u8f.setCursor(105, 110);
    u8f.print(days[rtc.getDoW()]);

    rtc.Conversion(0, rtc.getYear(), rtc.getMonth(), rtc.getDate());
    uint8_t moonT = 48;
    if (rtc.month_moon < 10) {
      moonT += 12;
    } else {
      moonT += 24;
    }

    if (rtc.day_moon < 10) {
      moonT += 12;
    } else {
      moonT += 24;
    }

    u8f.setCursor((240 - moonT) / 2, 130);
    u8f.printf("农历%s月%s日", moondays[rtc.month_moon] , moondays[rtc.day_moon]);


    for (int i = 0; i < 12; i++)
      if (start[i] + angle < 360) {
        img.drawString(cc[i], x[start[i] + angle], y[start[i] + angle], 2);
        img.drawLine(px[start[i] + angle], py[start[i] + angle], lx[start[i] + angle], ly[start[i] + angle], color1);
      }
      else
      {
        img.drawString(cc[i], x[(start[i] + angle) - 360], y[(start[i] + angle) - 360], 2);
        img.drawLine(px[(start[i] + angle) - 360], py[(start[i] + angle) - 360], lx[(start[i] + angle) - 360], ly[(start[i] + angle) - 360], color1);
      }




    img.setFreeFont(&DSEG7_Modern_Bold_20);
    img.drawString(s, sx, sy - 36);
    img.setFreeFont(&DSEG7_Classic_Regular_28);
    img.drawString(h + ":" + m, sx, sy + 33);
    img.setTextFont(0);

    img.fillRect(70, 90, 12, 20, color3);
    img.fillRect(84, 90, 12, 20, color3);
    img.fillRect(150, 90, 12, 20, color3);
    img.fillRect(164, 90, 12, 20, color3);

    img.setTextColor(0x35D7, TFT_BLACK);
    img.drawString("MONTH", 84, 78);
    img.drawString("DAY", 162, 78);
    img.setTextColor(TFT_ORANGE, TFT_BLACK);
    //img.drawString("VOLOS PROJECTS", 120, 174);
    //img.drawString("***", 120, 104);

    img.setTextColor(TFT_WHITE, color3);
    img.drawString(m1, 77, 100, 2);
    img.drawString(m2, 91, 100, 2);

    img.drawString(d1, 157, 100, 2);
    img.drawString(d2, 171, 100, 2);



    for (int i = 0; i < 60; i++)
      if (startP[i] + angle < 360)
        img.fillCircle(px[startP[i] + angle], py[startP[i] + angle], 1, color1);
      else
        img.fillCircle(px[(startP[i] + angle) - 360], py[(startP[i] + angle) - 360], 1, color1);

    img.fillTriangle(sx - 1, sy - 70, sx - 5, sy - 56, sx + 4, sy - 56, TFT_ORANGE);
    img.fillCircle(px[rAngle], py[rAngle], 6, TFT_RED);

    if (BAT_V) {
      int B = BAT_Votage();
      if(B>BAT_MAX)B = BAT_MAX;
      BAT_vol = map(B,  2600, BAT_MAX, 0, 100);
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

    BAT_display(BAT_vol);
    img.pushSprite(0, 0);
  }
}

void BAT_display(uint8_t b) {
  b = map(b, 100, 0, 0, 26);
  img.fillRect(106, 179, 4, 6, TFT_WHITE);
  img.fillRoundRect(108, 175, 28, 14, 3, TFT_WHITE);

  img.fillRoundRect(109, 176, 26, 12, 3, TFT_BLACK);
  if (b < 16)img.fillRoundRect(109 + b, 176, 26 - b, 12, 3, TFT_GREEN);
  else img.fillRoundRect(109 + b, 176, 26 - b, 12, 3, TFT_RED);

}
