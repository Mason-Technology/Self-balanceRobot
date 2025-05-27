#include <Arduino.h>
// --- Pin Definitions ---
const int ADC_PIN = 34;      // GPIO34 (ADC1_CH6) for battery voltage measurement
const int LOW_BATT_LED_PIN = 2; // GPIO2 (often the built-in LED on ESP32 DevKits)

// --- Voltage Divider Resistors ---
const float R1 = 10000.0; // 10k Ohms
const float R2 = 6200.0;  // 6.2k Ohms
const float MAX_BATTERY_VOLTAGE = 8.4;  // Voltage of a fully charged 2S LiPo (4.2V/cell)
const float MIN_BATTERY_VOLTAGE = 7.4;  // Cutoff voltage for a 2S LiPo (3.7V/cell)
const int CRITICAL_LOW_LEVEL_PERCENT = 10; // System stops if battery is below this
const int WARNING_LOW_LEVEL_PERCENT = 20;  // LED starts blinking if battery is below this
int adcRawValue;
float MAX_ADC_VALUE = 4095.0; // 12-bit ADC resolution (0-4095 for ESP32)
float ADC_REFERENCE_VOLTAGE = 3.2; 
int batteryLevelPercent;
long lastMillis = 0; 
const float VOLTAGE_DIVIDER_RATIO = (R1 + R2) / R2;// Pre-calculate voltage divider ratio
bool ledState = LOW; // For simple LED toggling

// Inseart your code pre-configuration here


void battCheckSetUp() {
  pinMode(LOW_BATT_LED_PIN, OUTPUT);
  digitalWrite(LOW_BATT_LED_PIN, LOW); // Start with LED off
}
void battCheck() {
  int adcRawValue = analogRead(ADC_PIN);  // 1. Read Raw ADC Value
  
  float voltageAtAdcPin = (adcRawValue / MAX_ADC_VALUE) * ADC_REFERENCE_VOLTAGE;// 2. Convert raw ADC to voltage at the ADC pin
  
  float currentBatteryVoltage = voltageAtAdcPin * VOLTAGE_DIVIDER_RATIO;// 3. Calculate original battery voltage
  
  // 4. Calculate Battery Level Percentage (Linear)
  if (currentBatteryVoltage >= MAX_BATTERY_VOLTAGE) {
    batteryLevelPercent = 100;
  } else if (currentBatteryVoltage <= MIN_BATTERY_VOLTAGE) {
    batteryLevelPercent = 0;
  } else {
    batteryLevelPercent = ((currentBatteryVoltage - MIN_BATTERY_VOLTAGE) / (MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE)) * 100.0;
  }

  if (millis() - lastMillis > 500) {
    lastMillis = millis(); // Reset lastMillis for next check


    if (batteryLevelPercent < CRITICAL_LOW_LEVEL_PERCENT) {
      ledState = !ledState; // Toggle LED state
      digitalWrite(LOW_BATT_LED_PIN, ledState);
    
    } else if (batteryLevelPercent < WARNING_LOW_LEVEL_PERCENT) {
        while (batteryLevelPercent < WARNING_LOW_LEVEL_PERCENT){
          ledState = !ledState; // Toggle LED state
          digitalWrite(LOW_BATT_LED_PIN, ledState);
          Serial.println("!! CRITICAL LOW BATTERY - SYSTEM HALT !!");
          Serial.print("Raw: "); Serial.print(adcRawValue);
          Serial.print(" | V_batt: "); Serial.print(currentBatteryVoltage, 2);
          Serial.print("V | Level: "); Serial.print(batteryLevelPercent); Serial.println("%");
          delay(500); // Blink every 500ms
        }
    }else{
      digitalWrite(LOW_BATT_LED_PIN, LOW); // Turn LED off if above warning level
      ledState = LOW; // Reset state
    }
    Serial.print("Raw: "); Serial.print(adcRawValue);
    Serial.print(" | V_batt: "); Serial.print(currentBatteryVoltage, 2);
    Serial.print("V | Level: "); Serial.print(batteryLevelPercent); Serial.println("%");
  }
}

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate
  battCheckSetUp();
  // Paste Your Setup Code here

}


void loop() {
  battCheck(); 
  
  // Paste Your Loop Code here

}