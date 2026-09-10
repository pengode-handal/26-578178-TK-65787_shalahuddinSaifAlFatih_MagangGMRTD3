# TUGAS GMRT MAGANG DAY 3 (ESP32 + MPU6050 + PIR)

Nama: Shalahuddin Saif Al Fatih<br>
NIM: 26/578178/TK/65787<br>
**FOLLOWING-AXIS SERVO MOTOR AND MOTION DETECTION**

## Hardware

| Komponen | Pin                  |
| -------- | -------------------- |
| Servo 1  | GPIO 12              |
| Servo 2  | GPIO 13              |
| Servo 3  | GPIO 25              |
| Servo 4  | GPIO 26              |
| Servo 5  | GPIO 27              |
| PIR      | GPIO 33              |
| MPU6050  | I2C (default `Wire`) |

Gyro range = `MPU6050_RANGE_250_DEG`, (set range rotasi -250deg/s - 250deg/s).

## Deskripsi

1. **Setup**: Init Serial, I2C, cek koneksi MPU6050 (retry loop tiap 1 detik kalau gagal), set gyro range, attach 5 servo, semua servo diarahkan ke posisi default 90 deg.
2. **Loop**: baca acc/gyro/temp dari MPU6050, lalu if statement:
    - **PIR HIGH (ada gerakan di sekitar sensor)** $\rightarrow$ semua servo di wirte ke 135 deg selama 500 ms, lalu kembali ke 90 deg dan hold 700 ms.
    - **PIR LOW** $\rightarrow$ servo mengikuti arah gyro lewat mapping roll/pitch/yaw.

## Mapping gyro $\rightarrow$ sudut servo

range gyro dikonversi dari deg/s ke rad/s:

$$\theta_{rad} = RANGE \\_ ROTATION \times \frac{\pi}{180} $$

karena fungsi bawaan `map()` itu di declare sbg `long map(long, long, long, long, long)` sementara `gyro.gyro.x/y/z` returnnya float yang bakal otomatis dipaksa jadi long ($\theta_{rad}=±4.363$) padahal mapping dari (anggap aja) -4 sampai 4 ke 0 sampai 180 itu beda decimalnya cukup ngaruh, jadi mapnya pakai fungsi manual dari google yang input parameter dan returnnya float:

$$\text{y} = (input - x_{min}) \cdot \frac{y_{max} - y_{min}}{x_{max} - x_{min}} + y_{min}$$

**Roll** (servo 1 & 2), muternya dibalik:

$$roll = 180 - 180 \cdot \frac{g_x + b}{2b}$$

**Pitch** (servo 3 & 4) dan **yaw** (servo 5), searah:

$$pitch = 180 \cdot \frac{g_y + b}{2b}, \qquad yaw = 180 \cdot \frac{g_z + b}{2b}$$

saat diam ($g = 0$), semua menghasilkan 90 deg.

## Khusus yaw (servo 5)

servo 5 hanya bergerak kalau:

- `gyro.gyro.z != 0`, **dan**
- nilai `yaw` hasil mapping berbeda dari `yawNow` (state terakhir).

Kalau kondisi terpenuhi: servo 5 di write ke sudut `yaw`, delay 1 detik, lalu di write balik ke 90 deg, dan `yawNow` diperbarui. `yawNow` dipakai biar servo 5 hanya gerak ketika input dari `gyro.gyro.z` beda. sebelumnya kucoba tanpa `yawNow` servo 5 bolak balik `yaw` - `default` - `yaw` - `default` harusnya ketika udah balik ke default maka servo 5 diam sampai ke input berbeda selanjutnya.
