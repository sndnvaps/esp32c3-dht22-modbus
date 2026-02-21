单片机使用 合宙ESP32C3经典版本(带调试串口)

使用modbus库为：modbus-esp8226,version:4.1.0

dht22/2302使用库：Bonezegei_DHT22,version:1.0.0

软件串口使用库:EspSoftwareSerial,version:8.1.0

dht22连接到 GPIO10

modbus参数
```
 BPS： 9600， 8N1
 Hreg, 16bit
 2位数据
 得到的数据需要 /10, 即缩少10倍
```
USB 转ttl刷机线（芯片使用CH340G),连接到板子上面的18,19端口
板上接口定义
```
   TX GPIO18
   RX GPIO19
   
TTL连线为
TTL_RX -> TX(GPIO18)
TTL_TX -> RX(GPIO19)
TTL_V3.3 -> 3V3
TTL_GND -> GND

BPS为：115200, 8N1
```
# 异常说明

void readDHT22AndConvert();
里面读取DHT22感觉有点问题，如果要上线使用，建议用usb转ttl刷机线，连接到 Arduino IDE上面，
查看一下数据是否正常。如果不正常，建议再换个温度传感器
