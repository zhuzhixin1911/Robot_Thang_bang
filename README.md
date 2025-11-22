# 🤖 Tên Dự Án: Robot Tự Cân Bằng Hai Bánh 
# (Self-Balancing Robot)


Dự án này là một **Robot Tự Cân Bằng Hai Bánh (Self-Balancing Robot)** được xây dựng trên nền tảng **Arduino**. Robot sử dụng cảm biến **MPU6050** để xác định độ nghiêng và áp dụng thuật toán **PID (Proportional-Integral-Derivative)** để điều chỉnh tốc độ động cơ, giữ cho robot luôn ở trạng thái thẳng đứng và tự cân bằng.

---

## ✨ Tính Năng Chính

* **Tự Cân Bằng:** Sử dụng cảm biến MPU6050 để đo góc nghiêng và tốc độ góc.
* **Điều Khiển Động Cơ:** Sử dụng module điều khiển động cơ **L298N** để cấp nguồn và điều khiển hai động cơ DC.
* **Bộ Điều Khiển PID:** Áp dụng thuật toán PID để đạt được sự cân bằng ổn định.

---

## 🛠️ Linh Kiện Cần Thiết (Bill of Materials)

Đây là các linh kiện chính được sử dụng trong dự án này, dựa trên sơ đồ mạch và hình ảnh robot:

| STT | Linh Kiện | Mô tả |
| :--- | :--- | :--- |
| 1 | **Arduino Uno** (hoặc tương đương) | Bộ vi điều khiển chính. |
| 2 | **MPU6050** (Gia tốc kế & Con quay hồi chuyển 6 trục) | Cảm biến góc nghiêng và tốc độ góc. |
| 3 | **L298N** Module Điều Khiển Động Cơ | Điều khiển hai động cơ DC (M1 và M2). |
| 4 | **Động Cơ DC** (Motor) | Hai động cơ DC có hộp số (Gear Motor), thường là loại BQ Motor. |
| 5 | **Bánh Xe** | Hai bánh xe. |
| 6 | **Khung Robot** | Khung cơ khí để lắp ráp các linh kiện. |
| 7 | **Nguồn Điện** (Pin/Acquy) | Cấp nguồn cho động cơ (7V) và Arduino (5V). |
| 8 | **Dây Nối** (Jumper Wires) | Dây để kết nối các module. |

---

## 🔌 Sơ Đồ Đấu Dây (Wiring Diagram)

Sơ đồ dưới đây minh họa cách kết nối các linh kiện chính:

![Sơ Đồ Đấu Dây Robot](/Hình_3.png)

### **Chi Tiết Kết Nối**

#### **1. Arduino & MPU6050 (Giao tiếp I2C)**

| Arduino Pin | MPU6050 Pin | Chức Năng |
| :--- | :--- | :--- |
| **A4 (SDA)** | **SDA** | Dữ liệu I2C |
| **A5 (SCL)** | **SCL** | Xung nhịp I2C |
| **5V** | **VCC** | Nguồn 5V |
| **GND** | **GND** | Nối đất |

#### **2. Arduino & L298N (Điều khiển Động Cơ)**

| Arduino Pin | L298N Pin | Chức Năng |
| :--- | :--- | :--- |
| **D9** | **IN1** | Điều khiển Động cơ M1 |
| **D8** | **IN2** | Điều khiển Động cơ M1 |
| **D7** | **IN3** | Điều khiển Động cơ M2 |
| **D6** | **IN4** | Điều khiển Động cơ M2 |
| **D10** | **ENA** | PWM Tốc độ Động cơ M1 |
| **D11** | **ENB** | PWM Tốc độ Động cơ M2 |
| **5V** | **VCC** | Nguồn 5V cho mạch logic của L298N |
| **GND** | **GND** | Nối đất |

* **Lưu ý:** L298N cũng cần nguồn 7V (hoặc cao hơn) để cấp cho động cơ, kết nối vào chân nguồn **7V** và **GND** của L298N.

---

### **Tinh Chỉnh (Tuning) PID**

Phần quan trọng nhất của dự án là tinh chỉnh các hệ số **Kp, Ki, Kd**. Quá trình này thường yêu cầu thử nghiệm lặp đi lặp lại để tìm ra sự kết hợp cho phản ứng nhanh và ổn định mà không bị dao động quá mức.

-----

## 🖼️ Hình Ảnh Dự Án

| Lắp Ráp Hoàn Chỉnh | Góc Nhìn Khác |
| :--- | :--- |
| ![Robot Tự Cân Bằng Góc 1](Hình_4.png) | ![Robot Tự Cân Bằng Góc 2](Hình_5.png) |

-----

## 🚀 Hướng Dẫn Cơ Bản

1.  **Lắp Ráp Cơ Khí:** Lắp ráp khung robot, động cơ, bánh xe và các tầng PCB (tham khảo các hình ảnh robot).
2.  **Đấu Dây:** Kết nối các linh kiện theo Sơ Đồ Đấu Dây.
3.  **Lập Trình:** Tải mã nguồn lên Arduino, đảm bảo các thư viện cần thiết đã được cài đặt.
4.  **Tinh Chỉnh PID:** Bắt đầu với các giá trị Kp, Ki, Kd thấp và tăng dần Kp cho đến khi robot có thể tự giữ thăng bằng. Sau đó, tinh chỉnh Ki và Kd để cải thiện độ ổn định và giảm sai số tĩnh.
