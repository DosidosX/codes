#include <WiFi.h>
#include <HTTPClient.h>

// --- WiFi ---
const char* ssid = "Flame";        // ใส่ WiFi ของคุณ
const char* password = "123456789";

// --- Telegram ---
String botToken = "8358092559:AAF0WY-t-prnSSd07402j30qT1VDQAgVNwk";
String chatID = "7692446760";

// --- Device ID ---
String deviceID = "ESP32_001";

// ประกาศฟังก์ชัน sendTelegram และ urlencode ก่อนการใช้งาน
void sendTelegram(String text);
String urlencode(String str);

void setup() {
  Serial.begin(115200);

  // เชื่อม WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

void loop() {
  // สุ่มค่าข้อมูล
  float temperature = random(200, 350) / 10.0; // 20.0 - 35.0 °C
  float humidity = random(400, 800) / 10.0;    // 40.0 - 80.0 %
  float voltage = random(300, 420) / 100.0;    // 3.00 - 4.20 V

  // สร้างข้อความ
  String msg = "📡 ข้อมูล ESP32 (สุ่ม)\n";
  msg += "ID: " + deviceID + "\n";
  msg += "อุณหภูมิ: " + String(temperature, 1) + " °C\n";
  msg += "ความชื้น: " + String(humidity, 1) + " %\n";
  msg += " แรงดัน: " + String(voltage, 2) + " V";

  // ส่งข้อความไป Telegram
  sendTelegram(msg);

  delay(10000); // ส่งทุก 10 วินาที
}

// --- ฟังก์ชันส่งข้อความ Telegram ---
void sendTelegram(String text) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // URL Telegram API
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatID + "&text=" + urlencode(text);

    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.println("ส่งข้อความสำเร็จ: " + text);
    } else {
      Serial.println("ส่งข้อความไม่สำเร็จ: " + http.errorToString(httpCode));
    }
    http.end();
  }
}

// --- ฟังก์ชัน URL Encode ---
String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) code0 = c - 10 + 'A';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}
