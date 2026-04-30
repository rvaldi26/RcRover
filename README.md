# 🏎️ RC Rover Cloud IoT (ESP8266 + MQTT)

Proyek ini adalah sistem kendali robot rover berbasis **Internet of Things (IoT)** menggunakan **NodeMCU ESP8266**. Robot ini dikendalikan secara nirkabel melalui dashboard web yang dideploy di **GitHub Pages** menggunakan protokol **MQTT (HiveMQ Cloud)**. Sistem ini dirancang untuk kemudahan konfigurasi tanpa harus melakukan pemrograman ulang berulang kali.

## 🌟 Fitur Utama
*   **Cloud-Based Control**: Kendali jarak jauh tanpa batas jarak selama tersedia koneksi internet.
*   **WiFiManager Integration**: Nama Access Point (AP) "RC Rover" memudahkan setup awal SSID WiFi dan Password.
*   **Dynamic MQTT Topic**: ID Rover (Root Topic) dapat diubah secara dinamis melalui portal WiFiManager dan tersimpan permanen di **LittleFS**.
*   **Real-time Telemetry**: Monitoring suhu (DHT22) dan deteksi kadar gas (MQ-135) secara langsung.
*   **Secure Connection**: Dashboard menggunakan protokol **WSS (WebSocket Secure)** pada port **8884** agar kompatibel dengan SSL GitHub Pages.

---

## 🛠️ Perkabelan & Koneksi (Wiring)

Gunakan tabel berikut sebagai panduan penyambungan komponen ke NodeMCU ESP8266:



### 1. Driver Motor L298N
| Pin NodeMCU | Pin L298N | Keterangan |
| :--- | :--- | :--- |
| **D1 (GPIO 5)** | IN1 | Motor A (Maju) |
| **D2 (GPIO 4)** | IN2 | Motor A (Mundur) |
| **D5 (GPIO 14)** | IN3 | Motor B (Maju) |
| **D6 (GPIO 12)** | IN4 | Motor B (Mundur) |

### 2. Sensor & Periferal
| Komponen | Pin NodeMCU | Keterangan |
| :--- | :--- | :--- |
| **DHT22** | **D3 (GPIO 0)** | Pin Data Sensor Suhu |
| **MQ-135** | **A0 (Analog)** | Pin Analog Sensor Gas |
| **Power** | Vin / 5V | Sumber tegangan (Baterai/Powerbank) |
| **GND** | GND | Hubungkan semua GND komponen (Common Ground) |

---

## ⚙️ Algoritma & Alur Kerja

Sistem ini bekerja dengan urutan logika sebagai berikut:



1.  **Booting & Configuration**: Saat aktif, perangkat mengecek file `config.txt` di **LittleFS**. Jika belum terhubung ke WiFi, NodeMCU mengaktifkan mode AP bernama **"RC Rover"**.
2.  **Portal Setup**: Pengguna memasukkan kredensial WiFi dan **ID Rover** (contoh: `rcrover01`) melalui portal web **tzapu WiFiManager**.
3.  **MQTT Bridge**: 
    *   **Control**: NodeMCU melakukan *subscribe* ke topik `[ID_Rover]/control`. Perintah teks ('F', 'B', 'L', 'R', 'S') diproses secara real-time.
    *   **Data**: NodeMCU melakukan *publish* data sensor dalam format **JSON** ke topik `[ID_Rover]/data` setiap 2 detik.
4.  **Web Interface**: Dashboard di GitHub Pages bertindak sebagai Client MQTT yang mengirimkan perintah kontrol dan mem-parsing data JSON sensor untuk ditampilkan secara visual.

---

## 🚀 Cara Instalasi

1.  **Arduino IDE Setup**:
    *   Instal board ESP8266 melalui Boards Manager.
    *   Instal Library: `WiFiManager` (tzapu), `PubSubClient`, `DHT sensor library`, dan `Adafruit Unified Sensor`.
2.  **Konfigurasi Flash**:
    *   Pilih **Tools > Flash Size > 4MB (FS:1MB)**.
3.  **Upload**: Masukkan kode `.ino` ke NodeMCU.
4.  **Deployment Dashboard**: 
    *   Unggah file `index.html` ke repositori GitHub.
    *   Aktifkan **GitHub Pages** di menu Settings repositori.
5.  **Penggunaan**: 
    *   Hubungkan HP ke WiFi "RC Rover" untuk setup awal.
    *   Buka URL GitHub Pages Anda, masukkan ID Rover yang telah diset, lalu klik **Connect**.

---
*Dikembangkan oleh **Rivaldi** - Komputer Sains, Medan.*
