void wifi_init() {
  int c = 0;
  drawwifi();
  u8f.setCursor(80, 180);
  u8f.print("正在连接Wi-Fi");
  Serial.println("******************************************************");
  Serial.print("Connecting to ");
  //Serial.println(WiFi.ssid);


  WiFi.begin();
  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
    tft.fillCircle(120, 120, 10, TFT_BLUE);
    delay(500);
    tft.fillCircle(120, 120, 10, TFT_RED);
    c++;
    u8f.setCursor(115, 200);
    u8f.print(c);
    if (c > 10) {
      drawwifi();
      u8f.setCursor(50, 160);
      u8f.print("连接超时，进入配网模式。");
      u8f.setCursor(50, 180);
      u8f.print(" 请使用乐鑫公众号配网");
      u8f.setCursor(50, 200);
      u8f.print("  等待接收配网信息...");
      WiFi.mode(WIFI_AP_STA);
      WiFi.beginSmartConfig(SC_TYPE_AIRKISS);
      Serial.println("Waiting for SmartConfig.");
      while (!WiFi.smartConfigDone()) {
        delay(500);
        Serial.print(".");
        tft.fillCircle(120, 120, 10, TFT_BLUE);
        delay(500);
        tft.fillCircle(120, 120, 10, TFT_BLACK);
      }
      Serial.println("");
      Serial.println("SmartConfig received.");
      Serial.println();
      Serial.println("******************************************************");
      Serial.print("Connecting to ");
      //Serial.println(ssid);
      drawwifi();
      u8f.setCursor(80, 180);
      u8f.print("正在连接Wi-Fi");
      c = 0;
    }
  }
  drawwifi();
  u8f.setCursor(80, 180);
  u8f.print("Wi-Fi已连接");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //esp_wifi_restore();
  delay(1000);
  tft.fillScreen(TFT_BLACK);
  u8f.setCursor(50, 100);
  u8f.print("开始进行网络对时...");

}

void drawwifi() {
  tft.fillScreen(TFT_BLACK);
  tft.fillCircle(120, 120, 10, TFT_BLUE);
  for (int i = 225; i < 315; i++) {
    int x = (40 * cos(rad * i)) + sx;
    int y = (40 * sin(rad * i)) + 120;
    tft.fillCircle(x, y, 10, TFT_BLUE);
  }
  for (int i = 225; i < 315; i++) {
    int x = (80 * cos(rad * i)) + sx;
    int y = (80 * sin(rad * i)) + 120;
    tft.fillCircle(x, y, 10, TFT_BLUE);
  }
  //  for(int i=225;i<315;i++){
  //    int x = (120 * cos(rad * i)) + sx;
  //    int y = (120 * sin(rad * i)) + 150;
  //    tft.fillCircle(x,y,10,TFT_BLUE);
  //  }
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return;
  }
  u8f.setCursor(50, 120);
  u8f.print("对时成功！当前时间：");
  u8f.setCursor(50, 140);
  u8f.printf("%d/%d/%d %d:%d:%d %d  \n", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_wday);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.printf("%d/%d/%d %d:%d:%d %d  \n", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, timeinfo.tm_wday);
  rtc.setRtcTime(timeinfo.tm_sec, timeinfo.tm_min, timeinfo.tm_hour, timeinfo.tm_wday , timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year - 100);
  rtc.setRtcTime(timeinfo.tm_sec, timeinfo.tm_min, timeinfo.tm_hour, timeinfo.tm_wday , timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year - 100);
  u8f.setCursor(50, 200);
  u8f.print(WiFi.macAddress());
}
