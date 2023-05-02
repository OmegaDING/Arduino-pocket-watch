static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;
 
    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
         Serial.println( "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        Serial.println("eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN, ADC_WIDTH_BIT_12, 0, &adc1_chars);
    } else {
       Serial.println("Invalid arg");
    }
 
    return cali_enable;
}

int BAT_Votage(){
  uint32_t sum = 0;
  uint32_t voltage;
  analogSetAttenuation(ADC_11db);
  for (size_t i = 0; i < 50; i++)
  {
    sum += analogRead(BAT_PIN);
    delay(1);
  }
  sum /= 50;
  Serial.print("Channel Raw Data=  ");
  Serial.println(sum);

  return sum;
}
