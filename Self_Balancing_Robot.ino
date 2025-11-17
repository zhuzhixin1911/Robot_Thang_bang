//Robot Tự Cân Bằng
#include <PID_v1.h>
#include <LMotorController.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
 #include "Wire.h"
#endif

#define MIN_ABS_SPEED 30

MPU6050 mpu;

// Các biến điều khiển/trạng thái MPU
bool dmpReady = false; // đặt thành true nếu khởi tạo DMP thành công
uint8_t mpuIntStatus; // giữ byte trạng thái ngắt thực tế từ MPU
uint8_t devStatus; // trạng thái trả về sau mỗi hoạt động của thiết bị (0 = thành công, !0 = lỗi)
uint16_t packetSize; // kích thước gói DMP dự kiến (mặc định là 42 byte)
uint16_t fifoCount; // đếm tất cả các byte hiện có trong FIFO
uint8_t fifoBuffer[64]; // bộ đệm lưu trữ FIFO

// Các biến về hướng/chuyển động
Quaternion q; // [w, x, y, z] bộ chứa quaternion
VectorFloat gravity; // [x, y, z] vector trọng lực
float ypr[3]; // [yaw, pitch, roll] bộ chứa yaw/pitch/roll và vector trọng lực

//PID
double originalSetpoint = 181.0;
double setpoint = originalSetpoint;
double movingAngleOffset = 0.1;
double input, output;

//điều chỉnh các giá trị này để phù hợp với thiết kế của riêng bạn
double Kp = 60;
double Kd = 2.2;
double Ki = 270;
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

double motorSpeedFactorLeft = 0.9;
double motorSpeedFactorRight = 0.9;

//BỘ ĐIỀU KHIỂN ĐỘNG CƠ
int ENA = 11;
int IN1 = 7;
int IN2 = 6;
int IN3 = 5;
int IN4 = 4;
int ENB = 10;
LMotorController motorController(ENA, IN1, IN2, ENB, IN3, IN4, motorSpeedFactorLeft, motorSpeedFactorRight);

volatile bool mpuInterrupt = false; // cho biết chân ngắt MPU đã lên mức cao hay chưa
void dmpDataReady()
{
 mpuInterrupt = true;
}


void setup()
{
 // tham gia bus I2C (thư viện I2Cdev không tự động làm điều này)
 #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
 Wire.begin();
 TWBR = 24; // xung nhịp I2C 400kHz (200kHz nếu CPU là 8MHz)
 #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
 Fastwire::setup(400, true);
 #endif

 mpu.initialize();

 devStatus = mpu.dmpInitialize();

 // cung cấp các giá trị offset gyro của riêng bạn ở đây, được điều chỉnh cho độ nhạy tối thiểu
 mpu.setXGyroOffset(220);
 mpu.setYGyroOffset(76);
 mpu.setZGyroOffset(-85);
 mpu.setZAccelOffset(1788); // 1688 là mặc định của nhà sản xuất cho chip thử nghiệm của tôi

 // đảm bảo nó hoạt động (trả về 0 nếu thành công)
 if (devStatus == 0)
 {
 // bật DMP, bây giờ nó đã sẵn sàng
 mpu.setDMPEnabled(true);

 // cho phép phát hiện ngắt Arduino
 attachInterrupt(0, dmpDataReady, RISING);
 mpuIntStatus = mpu.getIntStatus();

 // đặt cờ DMP Ready để hàm loop() chính biết rằng có thể sử dụng nó
 dmpReady = true;

 // lấy kích thước gói DMP dự kiến để so sánh sau này
 packetSize = mpu.dmpGetFIFOPacketSize();
 
 //cài đặt PID
 pid.SetMode(AUTOMATIC);
 pid.SetSampleTime(10);
 pid.SetOutputLimits(-255, 255);
 }
 else
 {
 // LỖI!
 // 1 = tải bộ nhớ ban đầu thất bại
 // 2 = cập nhật cấu hình DMP thất bại
 // (nếu nó sắp hỏng, thường mã lỗi sẽ là 1)
 Serial.print(F("Khởi tạo DMP thất bại (mã lỗi "));
 Serial.print(devStatus);
 Serial.println(F(")"));
 }
}


void loop()
{
 // nếu lập trình thất bại, không cố gắng làm bất cứ điều gì
 if (!dmpReady) return;

 // chờ ngắt MPU hoặc (các) gói bổ sung có sẵn
 while (!mpuInterrupt && fifoCount < packetSize)
 {
 //không có dữ liệu mpu - thực hiện tính toán PID và xuất ra động cơ
 pid.Compute();
 motorController.move(output, MIN_ABS_SPEED);
 
 }

 // đặt lại cờ ngắt và lấy byte INT_STATUS
 mpuInterrupt = false;
 mpuIntStatus = mpu.getIntStatus();

 // lấy số lượng FIFO hiện tại
 fifoCount = mpu.getFIFOCount();

 // kiểm tra tràn (điều này không bao giờ nên xảy ra trừ khi code của chúng ta quá kém hiệu quả)
 if ((mpuIntStatus & 0x10) || fifoCount == 1024)
 {
 // đặt lại để chúng ta có thể tiếp tục một cách sạch sẽ
 mpu.resetFIFO();
 Serial.println(F("Tràn FIFO!")); //FIFO overflow!

 // ngược lại, kiểm tra ngắt dữ liệu DMP đã sẵn sàng (điều này sẽ xảy ra thường xuyên)
 }
 else if (mpuIntStatus & 0x02)
 {
 // chờ độ dài dữ liệu khả dụng chính xác, thời gian chờ sẽ RẤT ngắn
 while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

 // đọc một gói từ FIFO
 mpu.getFIFOBytes(fifoBuffer, packetSize);
 
 // theo dõi số lượng FIFO ở đây phòng trường hợp có > 1 gói khả dụng
 // (điều này cho phép chúng ta đọc thêm ngay lập tức mà không cần chờ ngắt)
 fifoCount -= packetSize;

 mpu.dmpGetQuaternion(&q, fifoBuffer);
 mpu.dmpGetGravity(&gravity, &q);
 mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
 input = ypr[1] * 180/M_PI + 180;
 }
}