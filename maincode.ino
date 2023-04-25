
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h> 



char ssid[] = "B9-106";
char pass[] = "1mot2hai3ba";


char auth[] = "ohvacSXCOIJC6SbrSJK9eClqNGFhQlZ0";


#define DHT_PIN 4  // kết nối cảm biến dht11 với chân D4
#define DHT_TYPE DHT11
#define AIR_CONDITIONER_PIN 13  // pinout của máy lạnh là chân D13
#define HEATER_PIN 12           //pin out của máy sưởi là chân D12
#define DEHUMIDIFIERS_PIN 14    //pin out của máy hút ẩm là chân D14
#define HUMIDIFIER_PIN 27       //PIN OUT CỦA MÁY PHUN SƯƠNG LÀ CHÂN D27
 int VIRTUAL_PIN_1;       // virtual_pin của máy lạnh
 int VIRTUAL_PIN_2;        // virtual_pin của máy sưởi
 int VIRTUAL_PIN_3;         // virtual_pin của máy hút ẩm
 int VIRTUAL_PIN_4;        // virtual_pin của máy phun sươngint
 int VIRTUAL_PIN_5;        // virtual_pin tắt hết các thiết bị
 int VIRTUAL_PIN_6;       // virtual_pin chế độ điều khiển tự động

//Trạng thái các nút bấm trên app blynk
bool b1_isPushOnApp = false;
bool b2_isPushOnApp = false;
bool b3_isPushOnApp = false;
bool b4_isPushOnApp = false;
bool b5_isPushOnApp = false;
bool b6_isPushOnApp = false;

////Hàm ghi giá trị VitualPin V1 vào biến VIRTUAL_PIN_1 với kiểu nguyên
BLYNK_WRITE(V1) 
{
    VIRTUAL_PIN_1 = param.asInt();
    b1_isPushOnApp = true;
}
BLYNK_WRITE(V2) 
{
    VIRTUAL_PIN_2 = param.asInt();
    b2_isPushOnApp = true;
}
BLYNK_WRITE(V3) 
{
    VIRTUAL_PIN_3 = param.asInt();
    b3_isPushOnApp = true;
}
BLYNK_WRITE(V4) 
{
    VIRTUAL_PIN_4 =param.asInt();
    b4_isPushOnApp = true;
}
BLYNK_WRITE(V5) 
{
    VIRTUAL_PIN_5 = param.asInt();
    b5_isPushOnApp = true;
}
BLYNK_WRITE(V6) 
{
    VIRTUAL_PIN_6 = param.asInt();
    b6_isPushOnApp = true;
}


int humDht = 0;           //biến lưu giá trị độ ẩm
double tempDht = 0;          //biến lưu giá trị nhiệt độ
DHT dht(DHT_PIN, DHT_TYPE);  //Định nghĩa cảm biến DHT11
BlynkTimer timer;           //Định nghĩa timer

void setup() {
  Serial.begin(115200);
  //Khởi tạo các pin OUTPUT
  pinMode(AIR_CONDITIONER_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(DEHUMIDIFIERS_PIN, OUTPUT);
  pinMode(HUMIDIFIER_PIN, OUTPUT);
  //Set các OUTPUT ban đầu ở mức low
  digitalWrite(AIR_CONDITIONER_PIN, LOW);
  digitalWrite(HEATER_PIN, LOW);
  digitalWrite(DEHUMIDIFIERS_PIN, LOW);
  digitalWrite(HUMIDIFIER_PIN, LOW);
  

  dht.begin();                    // Bắt đầu đọc dữ liệu từ cảm biến DHT11
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);  //Khởi tạo kết nối với Blynk
  //Set thời gian timer cho các hàm dọc dữ liệu và điều khiển
  timer.setInterval(300, getDhtData);
  timer.setInterval(300, sendDhtData);
  timer.setInterval(100, ctrlDevices);
  timer.setInterval(100, ctrlOffDevices);
  timer.setInterval(100, autoCtrl);
 
}
void loop() {
  Blynk.run();  //Khởi chạy Blynk
  timer.run();  //Khởi chạy timer 
}
void getDhtData(void) {
  tempDht = dht.readTemperature();  //Đọc giá thị nhiệt độ
  humDht = dht.readHumidity();      //Đọc giá trị độ ẩm 
}
void sendDhtData(void) {
  Blynk.virtualWrite(V7, tempDht);  //Nhiệt độ với pin V10
  Blynk.virtualWrite(V8, humDht);   // Độ ẩm với pin V11
  
}

void autoCtrl(void) {
  if (VIRTUAL_PIN_6)  //Khi nhấn nút Auto trên App thì sẽ thực hiện điều khiển tự động
  {
    //Nhiệt độ lớn hơn 30oC thì điều hòa
    if (tempDht >= 30) {
      digitalWrite(AIR_CONDITIONER_PIN, 1);
      Blynk.virtualWrite(V1, 1);
    } else {
      digitalWrite(AIR_CONDITIONER_PIN, 0);
      Blynk.virtualWrite(V1, 0);
    }
    //Nhiệt độ nhỏ hơn 16oC thì bật máy sưởi.
    if (tempDht <= 16) {
      digitalWrite(HEATER_PIN, 1);
      Blynk.virtualWrite(V2, 1);
    } else {
      digitalWrite(HEATER_PIN, 0);
      Blynk.virtualWrite(V2, 0);
    }

    //độ ẩm lớn hơn 80% thì bật máy hút ẩm
    if (humDht >= 80) {
      digitalWrite(DEHUMIDIFIERS_PIN, 1);
      Blynk.virtualWrite(V3, 1);
    } else {
      digitalWrite(DEHUMIDIFIERS_PIN, 0);
      Blynk.virtualWrite(V3, 0);
    }
    //độ ẩm nhỏ hơn 35% thì bật máy phun sương
    if (humDht <= 35) {
      digitalWrite(HUMIDIFIER_PIN, 1);
      Blynk.virtualWrite(V4, 1);
    } else {
      digitalWrite(HUMIDIFIER_PIN, 0);
      Blynk.virtualWrite(V4, 0);
    }
  }
}
// Hàm điều khiển tắt tất cả các thiết bị một lúc
void ctrlOffDevices(void) {
  if (b5_isPushOnApp) {
    digitalWrite(AIR_CONDITIONER_PIN, 0);
    Blynk.virtualWrite(V1, 0);
    digitalWrite(HEATER_PIN, 0);
    Blynk.virtualWrite(V2, 0);
    digitalWrite(DEHUMIDIFIERS_PIN, 0);
    Blynk.virtualWrite(V3, 0);
    digitalWrite(HUMIDIFIER_PIN, 0);
    Blynk.virtualWrite(V4, 0);
    b5_isPushOnApp = false;
  }
}
void ctrlDevices(void) {
  if (b1_isPushOnApp) {
    digitalWrite(AIR_CONDITIONER_PIN, (VIRTUAL_PIN_1));
    b1_isPushOnApp = false;
  }
  if (b2_isPushOnApp) {
    digitalWrite(HEATER_PIN, (VIRTUAL_PIN_2));
    b2_isPushOnApp = false;
  }
  if (b3_isPushOnApp) {
    digitalWrite(DEHUMIDIFIERS_PIN, (VIRTUAL_PIN_3));
    b3_isPushOnApp = false;
  }
  if (b4_isPushOnApp) {
    digitalWrite(HUMIDIFIER_PIN, (VIRTUAL_PIN_4));
    b4_isPushOnApp = false;
  }
}


