#define ESP32

#if defined(ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"
#elif defined(ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "Flame"
#define WIFI_PASSWORD "123456789"

// InfluxDB Config
#define INFLUXDB_URL "http://172.20.10.2:8086"
#define INFLUXDB_TOKEN "AMicc-G_pIzlS28U1jJS0NUIv_RPevMnuLftmFym35ymfg-DYA06dt_6NfEhOpeTS1uHQ3aeYBkSan4ABY6rVg=="
#define INFLUXDB_ORG "25b28e3632e07784" // UUID แบบเต็ม
#define INFLUXDB_BUCKET "data1"

// Time zone info
#define TZ_INFO "UTC+7"

// Create InfluxDB client instance
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("sensor_data");

void setup() {
  Serial.begin(115200);

  // Setup WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected!");

  // Sync time (จำเป็นสำหรับ HTTPS & timestamp)
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check InfluxDB connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  // ตัวอย่างค่า sensor (สามารถอ่านจาก DHT/BMP280 จริงได้)
  float temperature = 28.5;
  float humidity = 65.2;
  float pressure = 1013.2;
  String device_id = "ESP32_001";

  // เตรียม point ส่งไป InfluxDB
  sensor.clearFields();
  sensor.clearTags();
  sensor.addTag("device", device_id);
  sensor.addField("temperature", temperature);
  sensor.addField("humidity", humidity);
  sensor.addField("pressure", pressure);

  // ส่งข้อมูล
  if (!client.writePoint(sensor)) {
    Serial.print("Write failed: ");
    Serial.println(client.getLastErrorMessage());
  } else {
    Serial.println("✅ Data sent successfully!");
  }

  delay(10000); // ส่งข้อมูลทุก 10 วินาที
}
