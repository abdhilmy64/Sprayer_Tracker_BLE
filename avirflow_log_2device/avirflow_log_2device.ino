#include <Arduino.h>
#include <WiFi.h>
#include "FS.h"
#include "SPIFFS.h"
#include "time.h"

const char* ssid = "Lantai 4 Deco";
const char* password = "B10ta_k3may0r4n!";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7 * 3600;  // GMT+7 (WIB)
const int   daylightOffset_sec = 0;

const int FLOW_PIN = 2;
const int BUZZER_PIN = 12;

volatile int COUNT = 0;
double FLOW_RATE = 0.0;
double TOTAL_VOLUME = 0.0;
double CURRENT_DOSE_VOLUME = 0.0;

const double ML_PER_PULSE = 1.596;
const double DOSE_THRESHOLD = 500.0;
const double DOSE_ALERT_THRESHOLD = 450.0;

bool dose_ready_to_log = false;
bool buzzer_on = false;

unsigned long previousMillis = 0;
const unsigned long interval = 1000;

unsigned long buzzerMillis = 0;
const unsigned long buzzerBlinkInterval = 300;

// GPS Dummy
double lat = -6.1647263;
double lon = 106.8102405;

// Fungsi ISR untuk Flow
void IRAM_ATTR FLOW_ISR() {
  COUNT++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Wifi for time
  WiFi.begin(ssid, password);
  Serial.print("Menyambung WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Tersambung!");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal mendapatkan waktu dari NTP");
    return;
  }
  Serial.printf("Waktu sekarang: %s", asctime(&timeinfo));

  // Flow meter
  pinMode(FLOW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), FLOW_ISR, RISING);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal mount SPIFFS");
    while (true);
  }

  Serial.println("Sistem siap");
}

void loop() {
  unsigned long currentMillis = millis();

  // Update Flow Rate setiap 1 detik
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    noInterrupts();
    FLOW_RATE = COUNT * ML_PER_PULSE;
    TOTAL_VOLUME += FLOW_RATE;
    CURRENT_DOSE_VOLUME += FLOW_RATE;
    COUNT = 0;
    interrupts();

    updateGPS();

    Serial.printf("FlowRate: %.2f mL/min, Dose: %.2f mL, Total: %.2f mL\n",
                  FLOW_RATE, CURRENT_DOSE_VOLUME, TOTAL_VOLUME);

    // Jika mendekati dosis, aktifkan buzzer berkedip
    if (CURRENT_DOSE_VOLUME >= DOSE_ALERT_THRESHOLD && !dose_ready_to_log) {
      buzzer_on = true;
    }

    // Jika sudah melebihi ambang & flow berhenti
    if (CURRENT_DOSE_VOLUME >= DOSE_THRESHOLD && FLOW_RATE == 0.0 && !dose_ready_to_log) {
      dose_ready_to_log = true;
      logDose();
      resetDose();
      printLogFile();
    }
  }

  handleBuzzer(currentMillis);
}

// Simulasi pergerakan lokasi
void updateGPS() {
  // Tambah sedikit setiap detik
  lat += 0.000005;
  lon += 0.000007;
}

// Handle buzzer blinking
void handleBuzzer(unsigned long now) {
  if (buzzer_on) {
    if (now - buzzerMillis >= buzzerBlinkInterval) {
      buzzerMillis = now;
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN)); // toggle
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// Logging ke file CSV
void logDose() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal dapat waktu log");
    return;
  }

  File file = SPIFFS.open("/dose_log.csv", FILE_APPEND);
  if (!file) {
    Serial.println("Gagal membuka file log");
    return;
  }

  if (file.size() == 0) {
    file.println("datetime,latitude,longitude,dose_volume,total_volume");
  }

  char timeStr[30];
  strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &timeinfo);

  file.printf("%s,%.7f,%.7f,%.2f,%.2f\n",
              timeStr, lat, lon, CURRENT_DOSE_VOLUME, TOTAL_VOLUME);

  file.close();
  Serial.println(">>> Dosis tercatat ke file CSV <<<");
}

// Reset variabel untuk dosis selanjutnya
void resetDose() {
  CURRENT_DOSE_VOLUME = 0.0;
  dose_ready_to_log = false;
  buzzer_on = false;
}

void printLogFile() {
  File file = SPIFFS.open("/dose_log.csv", "r");
  if (!file) {
    Serial.println("Gagal membuka file.");
    return;
  }

  Serial.println("--- ISI FILE /dose_log.csv ---");
  while (file.available()) {
    Serial.write(file.read()); // kirim byte demi byte ke Serial Monitor
  }
  Serial.println("\n--- SELESAI ---");
  file.close();
}
