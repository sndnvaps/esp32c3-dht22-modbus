/*
  ModbusRTU ESP8266/ESP32
  Simple slave example

  (c)2019 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266

  modified 13 May 2020
  by brainelectronics
  modified 21 Feb 2026
  by sndnvaps

  This code is licensed under the BSD New License. See LICENSE.txt for more info.
*/
#include <Bonezegei_DHT22.h>
#include <ModbusRTU.h>

#include <SoftwareSerial.h>

#define TX (18)
#define RX (19)
#define BAUD_RATE 115200

#define SLAVE_ID 1

// DHT22 配置
#define DHTPIN 10         // DHT22/2302 数据引脚
#define REG_TEMPERATURE 0  // 温度×10 寄存器地址
#define REG_HUMIDITY 1     // 湿度×10 寄存器地址

EspSoftwareSerial::UART softSerial;
Bonezegei_DHT22 dht(DHTPIN);
ModbusRTU mb;

// 存储温湿度整数数据
int16_t temp_int = 0;
int16_t humi_int = 0;

// -------------------------- 函数声明 --------------------------
void readDHT22AndConvert();
void updateModbusRegisters();

void setup() {
  Serial.begin(9600, SERIAL_8N1);
  softSerial.begin(BAUD_RATE, EspSoftwareSerial::SWSERIAL_8N1, RX, TX);
#if defined(ESP32) || defined(ESP8266)
  mb.begin(&Serial);
#else
  mb.begin(&Serial);

  mb.setBaudrate(9600);
#endif
  softSerial.println("start modbus slave!");
  mb.slave(SLAVE_ID);
  mb.addHreg(REG_TEMPERATURE);
  mb.addHreg(REG_HUMIDITY);

  dht.begin();
}

void loop() {
  // 每隔2秒更新一次 DHT22 数据
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 2000) {
    lastUpdate = millis();
    readDHT22AndConvert();    // 读取并转换数据
    updateModbusRegisters();  // 更新 Modbus 寄存器
  }

  // 处理 Modbus 客户端请求（核心，必须循环调用）
  mb.task();
}

/**
 * 读取 DHT22 数据并转为 ×10 的整数，带异常处理
 */
void readDHT22AndConvert() {

  int humidity = 0;
  float temperature = 0.0;
  if (dht.getData()) {
    softSerial.println(" DHT22 数据读取成功！");
    temperature = dht.getTemperature();
    humidity = dht.getHumidity();
    // 放大10倍转整数（避免浮点型，简化 Modbus 处理）

    /*
    //BUG: FIXME
    此为放大倍数有点问题，我的DHT22读取的数据个位数，没有大于10的，
    所以就乘100，才能得到需要的结果
    */
    temp_int = (int16_t)(temperature*100);
    humi_int = (int16_t)(humidity*100);
    // 打印调试信息
    softSerial.print("温度(×10)：");
    softSerial.print(temp_int);
    softSerial.print(" | 湿度(×10)：");
    softSerial.println(humi_int);
    return;
  }
  softSerial.println(" DHT22 数据读取失败！");
  temp_int = 0;
  humi_int = 0;
}

/**
 * 将整数温湿度写入 Modbus 保持寄存器
 */
void updateModbusRegisters() {
  // 直接写入寄存器（ModbusIP_ESP8266 库原生支持）
  softSerial.println("写入Modbus HoldingReg!");
  mb.Hreg(REG_TEMPERATURE, temp_int);
  mb.Hreg(REG_HUMIDITY, humi_int);
}