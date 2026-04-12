#include <Wire.h>                // I2C communication (LCD এর জন্য)
#include <LiquidCrystal_I2C.h>  // LCD control library
#include <SoftwareSerial.h>     // ESP32 এর সাথে serial communication

// Sensors (3টা flame + 3টা gas)
int flame1 = 2, flame2 = 3, flame3 = 4;  // Fire sensors pins
int gas1 = 5, gas2 = 6, gas3 = 7;        // Gas sensors pins

// Outputs (alarm)
int led = 8, buzzer = 9;                 // LED & buzzer pins

// ESP32 serial (UNO → ESP32 communication)
#define ESP_RX 10                        // UNO RX (receive)
#define ESP_TX 11                        // UNO TX (send)
SoftwareSerial espSerial(ESP_RX, ESP_TX); // Software serial object

// LCD setup (I2C address, column, row)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// SMS timing control
bool msgSent = false;                   // SMS already sent?
unsigned long lastMsgTime = 0;          // last send time
const unsigned long msgInterval = 60000; // 60 sec delay

// ================= SETUP =================
void setup() {
  Serial.begin(9600);          // PC serial monitor
  espSerial.begin(9600);       // ESP32 communication

  lcd.init();                  // LCD start
  lcd.backlight();             // LCD light ON
  lcd.setCursor(0,0);          
  lcd.print("Fire Safety System"); // show title
  delay(1500);                 // wait 1.5 sec

  // Sensor pins INPUT
  pinMode(flame1, INPUT); 
  pinMode(flame2, INPUT); 
  pinMode(flame3, INPUT);
  pinMode(gas1, INPUT); 
  pinMode(gas2, INPUT); 
  pinMode(gas3, INPUT);

  // Output pins
  pinMode(led, OUTPUT); 
  pinMode(buzzer, OUTPUT);

  // Initially OFF
  digitalWrite(led, LOW); 
  digitalWrite(buzzer, LOW);
}

// ================= LOOP =================
void loop() {

  // 🔍 Sensor read
  int f1 = digitalRead(flame1);
  int f2 = digitalRead(flame2);
  int f3 = digitalRead(flame3);
  int g1 = digitalRead(gas1);
  int g2 = digitalRead(gas2);
  int g3 = digitalRead(gas3);

  // 🔥 LOW মানে fire detected
  bool fireBlocks[3] = {f1 == LOW, f2 == LOW, f3 == LOW};

  // 💨 LOW মানে gas detected
  bool gasBlocks[3]  = {g1 == LOW, g2 == LOW, g3 == LOW};

  // 🔢 কয়টা জায়গায় fire আছে
  int fireCount = fireBlocks[0] + fireBlocks[1] + fireBlocks[2];

  // 💨 যেকোনো জায়গায় gas আছে কিনা
  bool anyGas = gasBlocks[0] || gasBlocks[1] || gasBlocks[2];

  // ================= LCD LINE 0 =================
  String line0 = "";   // summary text

  if (fireCount > 0) {
    line0 += "Fire:";  // Fire label
    if (fireBlocks[0]) line0 += " A"; // Zone A fire
    if (fireBlocks[1]) line0 += " B"; // Zone B fire
    if (fireBlocks[2]) line0 += " C"; // Zone C fire
  }

  if (anyGas) {
    if (line0 != "") line0 += " "; // space add
    line0 += "Gas:";               // Gas label
    if (gasBlocks[0]) line0 += " A";
    if (gasBlocks[1]) line0 += " B";
    if (gasBlocks[2]) line0 += " C";
  }

  // ================= INSTRUCTION LINE =================
  String line3 = "";

  if (fireCount == 1) {
    // ১টা fire হলে safe stairs suggest
    if (fireBlocks[0]) line3 = "Use stairs B&C";
    else if (fireBlocks[1]) line3 = "Use stairs A&C";
    else if (fireBlocks[2]) line3 = "Use stairs A&B";
  } 
  else if (fireCount == 2) {
    // ২টা fire হলে শুধু safe zone
    if (!fireBlocks[0]) line3 = "Use stairs A";
    else if (!fireBlocks[1]) line3 = "Use stairs B";
    else if (!fireBlocks[2]) line3 = "Use stairs C";
  } 
  else if (fireCount == 3) {
    line3 = "RIP"; // সব জায়গায় fire
  } 
  else if (anyGas) {
    line3 = "Check area"; // শুধু gas
  }

  // 🚨 Danger detect
  bool dangerDetected = (fireCount > 0 || anyGas);

  // ================= DANGER MODE =================
  if (dangerDetected) {

    digitalWrite(led, HIGH);     // LED ON
    digitalWrite(buzzer, HIGH);  // buzzer ON

    // 📩 SMS message তৈরি
    String smsText = "";

    if (fireCount > 0) {
      smsText += "Fire on:";
      if (fireBlocks[0]) smsText += " A";
      if (fireBlocks[1]) smsText += " B";
      if (fireBlocks[2]) smsText += " C";
      smsText += ". ";

      if (fireCount < 3) smsText += line3 + ".";
      else smsText += "No stairs available!";
    }

    if (anyGas) {
      smsText += " Gas leak on:";
      if (gasBlocks[0]) smsText += " A";
      if (gasBlocks[1]) smsText += " B";
      if (gasBlocks[2]) smsText += " C";
      smsText += ".";
    }

    // ================= LCD UPDATE =================
    lcd.clear();
    lcd.setCursor(0,0); lcd.print(line0);

    lcd.setCursor(0,1);
    lcd.print("F1:"); lcd.print(fireBlocks[0] ? "Flame" : "No");
    lcd.print(" F2:"); lcd.print(fireBlocks[1] ? "Flame" : "No");
    lcd.print(" F3:"); lcd.print(fireBlocks[2] ? "Flame" : "No");

    lcd.setCursor(0,2);
    lcd.print("G1:"); lcd.print(gasBlocks[0] ? "Gas" : "OK");
    lcd.print(" G2:"); lcd.print(gasBlocks[1] ? "Gas" : "OK");
    lcd.print(" G3:"); lcd.print(gasBlocks[2] ? "Gas" : "OK");

    lcd.setCursor(0,3); lcd.print(line3);

    // ================= ESP32 SEND =================
    unsigned long currentTime = millis();

    if (!msgSent || currentTime - lastMsgTime >= msgInterval) {
      espSerial.println(smsText);   // ESP32-তে পাঠানো
      msgSent = true;
      lastMsgTime = currentTime;
    }

    Serial.println("ALERT: " + smsText); // debug

  } 
  // ================= NORMAL MODE =================
  else {

    digitalWrite(led, LOW);     // LED OFF
    digitalWrite(buzzer, LOW);  // buzzer OFF

    msgSent = false;            // reset SMS flag

    lcd.clear();
    lcd.setCursor(0,0); lcd.print("Status: Normal");
    lcd.setCursor(0,1); lcd.print("F1:No F2:No F3:No");
    lcd.setCursor(0,2); lcd.print("G1:OK G2:OK G3:OK");
    lcd.setCursor(0,3); lcd.print("ESP32 Link Active");
  }

  delay(500); // 0.5 sec delay
}