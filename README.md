Program ini digunakan untuk **memantau berat dan tinggi badan anak secara otomatis**, kemudian menampilkan data di:

* **LCD 16x2 I2C**, dan
* **Aplikasi Blynk IoT (di smartphone)**

Selain itu, sistem akan **memberi peringatan (alert)** jika hasil pengukuran berat atau tinggi **tidak sesuai standar normal anak sehat** (berpotensi stunting).

---

## ⚙️ **2. Komponen yang Digunakan**

| Komponen                  | Fungsi                                                    |
| ------------------------- | --------------------------------------------------------- |
| NodeMCU ESP8266           | Otak sistem dan pengirim data ke Blynk                    |
| HX711 + Load Cell         | Mengukur **berat badan** anak                             |
| Ultrasonic Sensor HC-SR04 | Mengukur **tinggi badan** anak                            |
| LCD I2C 16x2              | Menampilkan berat dan tinggi secara langsung              |
| Aplikasi Blynk            | Memantau data berat dan tinggi secara online melalui WiFi |

---

## 🔌 **3. Wiring / Skema Koneksi**

### 🟢 **a. Load Cell + HX711 Module**

| HX711 Pin | NodeMCU Pin |
| --------- | ----------- |
| VCC       | 3.3V        |
| GND       | GND         |
| DT (DOUT) | D3          |
| SCK       | D4          |

> Load cell dihubungkan ke terminal HX711 sesuai warna kabel (E+, E-, A+, A-).

---

### 🔵 **b. Ultrasonic Sensor HC-SR04**

| Pin  | NodeMCU Pin |
| ---- | ----------- |
| VCC  | 5V          |
| GND  | GND         |
| Trig | D5          |
| Echo | D6          |

---

### 🟣 **c. LCD I2C 16x2**

| Pin LCD I2C | NodeMCU Pin |
| ----------- | ----------- |
| VCC         | 5V          |
| GND         | GND         |
| SDA         | D2          |
| SCL         | D1          |

---

### 🟡 **d. NodeMCU Power**

Gunakan adaptor 5V 2A (misal charger HP) melalui port micro USB NodeMCU.

---

## 🧩 **4. Penjelasan Alur Program**

### 🟩 **Bagian Inisialisasi**

```cpp
#define BLYNK_TEMPLATE_ID "..."
#define BLYNK_AUTH_TOKEN "..."
#include <HX711.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
```

* Library di atas berfungsi untuk koneksi Blynk, membaca load cell, menampilkan LCD, dan koneksi WiFi.

Kemudian:

```cpp
char ssid[] = "aku";
char pass[] = "12345678910";
```

→ WiFi yang digunakan agar NodeMCU bisa terhubung ke internet dan mengirim data ke Blynk.

---

### 🟩 **Bagian setup()**

```cpp
Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
lcd.init();
lcd.backlight();
scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
scale.set_scale();
scale.tare();
```

* Menghubungkan NodeMCU ke **WiFi + Blynk Cloud**.
* Menyiapkan LCD dan menyalakan lampu belakang.
* Menginisialisasi **sensor berat (HX711)** dan men-**tare (mengatur nol)**.

---

### 🟩 **Bagian loop()**

#### 1. Jalankan Blynk

```cpp
Blynk.run();
```

→ Agar NodeMCU terus mengirim data ke server Blynk secara real-time.

---

#### 2. Baca berat badan

```cpp
float weight = scale.get_units(10);
```

→ Membaca berat dengan rata-rata dari 10 kali pembacaan.

---

#### 3. Baca tinggi badan

```cpp
digitalWrite(TRIG_PIN, HIGH);
delayMicroseconds(10);
digitalWrite(TRIG_PIN, LOW);
duration = pulseIn(ECHO_PIN, HIGH);
distance = (duration / 2) / 29.1;
```

→ Sensor ultrasonik mengukur jarak pantulan gelombang suara (dalam cm).
Jika sensor ditempatkan di atas kepala anak, maka hasil jarak bisa dikonversi ke tinggi badan (misalnya tinggi total - jarak sensor ke kepala).

---

#### 4. Tampilkan di LCD

```cpp
lcd.print("Weight: ");
lcd.print(weight);
lcd.print("g");
lcd.setCursor(0,1);
lcd.print("Height: ");
lcd.print(distance);
lcd.print("cm");
```

→ Berat dan tinggi tampil di layar LCD secara langsung.

---

#### 5. Kirim ke Blynk App

```cpp
Blynk.virtualWrite(V0, weight);
Blynk.virtualWrite(V1, distance);
```

→ Mengirimkan data ke **Virtual Pin V0 dan V1** di aplikasi Blynk.
Biasanya ditampilkan dalam **widget gauge atau value display**.

---

#### 6. Cek status normal/tidak

```cpp
if (weight < minWeight || weight > maxWeight || distance < minHeight || distance > maxHeight)
```

→ Jika data **di luar batas normal**, maka:

* Menampilkan pesan peringatan di LCD
* Mengirimkan **notifikasi (event log)** ke Blynk

---

## 📱 **5. Tampilan di Aplikasi Blynk**

Di dashboard Blynk (mobile app atau web dashboard):

| Widget                | Virtual Pin | Fungsi                                         |
| --------------------- | ----------- | ---------------------------------------------- |
| Gauge / Value Display | V0          | Menampilkan berat (gram)                       |
| Gauge / Value Display | V1          | Menampilkan tinggi (cm)                        |
| Notification          | Event log   | Menampilkan peringatan “Check Child’s Health!” |

---

## ⚠️ **6. Catatan Penting**

1. **Kalibrasi HX711:**
   Gunakan fungsi `scale.set_scale(XXX);` dengan faktor kalibrasi hasil pengujian agar berat terbaca akurat.
2. **Posisi sensor ultrasonik:**
   Pasang di **atas** (menghadap ke bawah), lalu hitung tinggi anak = tinggi total - jarak sensor ke kepala.
3. **Koneksi WiFi stabil:**
   Blynk hanya bekerja jika WiFi tersambung dengan baik.
4. **Perhatikan batas daya:**
   Pastikan NodeMCU dan semua sensor mendapatkan suplai 5V stabil dari adaptor minimal 2A.

---

## 🔧 **7. Gambaran Singkat Alur Kerja**

➡️ Anak berdiri di atas **load cell**
➡️ Sensor HX711 menghitung berat badan
➡️ Sensor ultrasonik membaca tinggi badan
➡️ Data ditampilkan di **LCD 16x2**
➡️ Data dikirim ke **Blynk Cloud** via WiFi
➡️ Jika hasil di luar batas normal → **Alert di LCD & Blynk App**


