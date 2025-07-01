# AirFlow Logger dengan Dual Device

Proyek ini adalah sistem monitoring aliran air menggunakan ESP32 yang dilengkapi dengan fitur pencatatan dosis, GPS tracking, dan notifikasi buzzer.

## Fitur Utama

- Monitoring aliran air secara real-time
- Perhitungan volume air (flow rate dan total volume)
- Sistem dosis dengan threshold 500mL
- Notifikasi buzzer saat mendekati batas dosis (450mL)
- Pencatatan data ke file CSV di SPIFFS
- Sinkronisasi waktu menggunakan NTP
- Tracking lokasi (simulasi GPS)

## Spesifikasi Hardware

- Mikrokontroler: ESP32
- Sensor: Flow meter (terhubung ke pin 2)
- Buzzer: Terhubung ke pin 12
- Koneksi: WiFi

## Konfigurasi

```cpp
const double ML_PER_PULSE = 1.596;     // Kalibrasi flow meter
const double DOSE_THRESHOLD = 500.0;    // Batas dosis (mL)
const double DOSE_ALERT_THRESHOLD = 450.0;  // Batas peringatan (mL)
```

## Format Data Log

Data dicatat dalam format CSV dengan kolom berikut:
- datetime: Waktu pencatatan (YYYY-MM-DD HH:MM:SS)
- latitude: Koordinat lintang
- longitude: Koordinat bujur
- dose_volume: Volume dosis terakhir (mL)
- total_volume: Total volume keseluruhan (mL)

## Cara Kerja

1. Sistem melakukan koneksi ke WiFi dan sinkronisasi waktu dengan NTP server
2. Flow meter menghitung aliran air melalui interrupt
3. Setiap detik, sistem:
   - Menghitung flow rate dan volume
   - Mengupdate posisi GPS (simulasi)
   - Memeriksa threshold dosis
4. Ketika volume mencapai 450mL, buzzer akan berkedip sebagai peringatan
5. Ketika volume mencapai 500mL dan aliran berhenti, sistem akan:
   - Mencatat data ke file CSV
   - Mereset perhitungan dosis
   - Menampilkan isi file log

## Pengembangan

Kode ini menggunakan beberapa library Arduino:
- Arduino.h
- WiFi.h
- FS.h
- SPIFFS.h
- time.h
