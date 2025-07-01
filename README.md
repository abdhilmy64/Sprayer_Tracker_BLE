# Sprayer Tracker BLE

Sprayer Tracker BLE adalah proyek berbasis ESP32 yang berfungsi untuk memonitor aliran cairan (flow rate), totalizer, serta posisi GPS dan status baterai/fuel melalui koneksi Bluetooth Low Energy (BLE). Proyek ini cocok digunakan untuk aplikasi seperti monitoring alat semprot pertanian atau industri.

## Fitur Utama
- Pengiriman data flow rate, totalizer, latitude, dan longitude secara periodik melalui BLE
- Pengiriman status baterai dan persentase bahan bakar (fuel) melalui BLE
- Kompatibel dengan ESP32 (menggunakan FreeRTOS)
- Payload data dikirim dalam format HEX

## File dan Struktur Folder
- `Sprayer_Tracker_BLE.ino` : Program utama ESP32 BLE untuk monitoring flow, GPS, baterai, dan fuel
- `Flow Program Sprayer` : Diagram alur program (format draw.io)
- `avirflow_log_2device/` : Submodul logger airflow dengan fitur pencatatan ke CSV, NTP, dan WiFi (lihat README di dalam folder)
- `Untitled Diagram.drawio` : Diagram tambahan (opsional)

## Cara Kerja Singkat
1. ESP32 akan menginisialisasi BLE dan membuat dua karakteristik:
   - Karakteristik 1: mengirim data flow rate, totalizer, latitude, longitude
   - Karakteristik 2: mengirim data status baterai dan fuel
2. Data dikirim secara periodik (setiap 1 detik) ke client BLE yang terhubung
3. Data dapat dimonitor melalui aplikasi BLE client di smartphone/PC

## Hardware yang Dibutuhkan
- ESP32 (disarankan seri C3/C6)
- Sensor flow meter (opsional, pada contoh menggunakan data dummy)
- Sumber daya (baterai)

## Instalasi Library Arduino
Pastikan library berikut sudah terpasang di Arduino IDE:
- `ESP32 BLE Arduino`

## Cara Upload
1. Buka file `Sprayer_Tracker_BLE.ino` di Arduino IDE
2. Pilih board ESP32 yang sesuai
3. Upload ke board

## Lisensi
Proyek ini open source dan dapat dikembangkan lebih lanjut sesuai kebutuhan.
