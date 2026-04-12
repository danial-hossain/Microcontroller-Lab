#include <WiFi.h>          // WiFi connect করার জন্য library
#include <HTTPClient.h>    // HTTP request (Telegram API) পাঠানোর জন্য

// WiFi credentials
const char* ssid = "lucas";        // তোমার WiFi নাম
const char* password = "01838883395@@@"; // WiFi password

// Telegram Bot information
String botToken = "8694751767:AAHxAb7Xlq06rAOzjoAD3uKD6qGGWqVVPJM"; // Bot token
String chatID  = "7565771938";   // তোমার chat ID

// UNO ↔ ESP32 serial communication (UART2)
HardwareSerial unoSerial(2); // UART2 ব্যবহার

// ================= SETUP =================
void setup() {

  Serial.begin(115200); // Serial monitor (debug দেখার জন্য)

  // UNO এর সাথে serial শুরু (RX=16, TX=17)
  unoSerial.begin(9600, SERIAL_8N1, 16, 17);

  Serial.println("Connecting WiFi..."); // debug message

  WiFi.begin(ssid, password); // WiFi connect শুরু

  // যতক্ষণ না connect হচ্ছে, অপেক্ষা করবে
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); // progress দেখায়
  }

  Serial.println("\nWiFi connected!"); // connect success

  // system start হলে Telegram এ message পাঠায়
  sendTelegram("🚨 Fire System Online");
}

// ================= LOOP =================
void loop() {

  // UNO থেকে data আসছে কিনা check
  if (unoSerial.available()) {

    // UNO থেকে পুরো message পড়া (newline পর্যন্ত)
    String msg = unoSerial.readStringUntil('\n');

    msg.trim(); // extra space/newline remove

    // যদি message empty না হয়
    if (msg.length() > 0) {

      sendTelegram(msg); // Telegram এ পাঠানো

      // Serial monitor এ দেখানো
      Serial.println("Sent to Telegram: " + msg);
    }
  }
}

// ================= TELEGRAM FUNCTION =================
void sendTelegram(String msg) {

  // WiFi connected কিনা check
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http; // HTTP object তৈরি

    // Telegram API URL তৈরি
    String url = "https://api.telegram.org/bot" + botToken +
                 "/sendMessage?chat_id=" + chatID +
                 "&text=" + msg;

    http.begin(url); // request start
    http.GET();      // GET request send (message যাবে)
    http.end();      // connection close
  }
}