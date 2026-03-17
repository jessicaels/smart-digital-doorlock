#include <WiFi.h>
#include <HTTPClient.h>
#include <MFRC522.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>

// --- WiFi Configuration ---
const char* ssid = "F";
const char* password = "0123456789";
const char* scriptURL = "https://script.google.com/macros/s/AKfycbzogQAhfQ1RL7VhEzaxUjDXQYTutDkeQcQgbYPzuZSBJwNJseVUXM_JvvsHTsCFVFwj/exec";

// --- RFID RC522 Configuration ---
#define RST_PIN 2
#define SS_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- LCD I2C Configuration ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Output Pins ---
#define RELAY_PIN 15
#define BUZZER_PIN 13

// --- Authorized UID ---
byte validUID[4] = {0xE3, 0xBE, 0x69, 0x30}; // UID pin kartu RFID
int failedAttempts = 0;
String uidToString(byte *uid);

// --- Anti-bounce Protection ---
unsigned long lastScanTime = 0;
const unsigned long debounceDelay = 3000; // 3 second cooldown
bool isProcessing = false;

void setup() {
  Serial.begin(115200);

  // Initialize Components
  SPI.begin(18, 19, 23, SS_PIN); // SCK, MISO, MOSI, SS
  mfrc522.PCD_Init();
  lcd.clear();
  lcd.begin();
  lcd.backlight();
 

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // Default: locked

  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("Menginisialisasi");
  lcd.setCursor(0, 1);
  lcd.print("Doorlock System");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menghubungkan ke");
  lcd.setCursor(0, 1);
  lcd.print("WiFi...");
 
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
	delay(500);
	lcd.setCursor(10 + dots, 1);
	lcd.print(".");
	dots = (dots + 1) % 3;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Terhubung!");
  lcd.setCursor(0, 1);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());
  delay(2000);
  lcd.clear();
}

void loop() {
  // Don't proceed if currently processing
  if (isProcessing) return;

  // If not enough time has passed since last scan
  if (millis() - lastScanTime < debounceDelay) {
	displayReadyMessage();
	return;
  }

  // Wait for card
  if (!mfrc522.PICC_IsNewCardPresent()) {
	displayReadyMessage();
	return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // Mark that we're processing a card
  isProcessing = true;
  displayProcessing();

  // Read UID
  byte readUID[4];
  for (byte i = 0; i < 4; i++) {
	readUID[i] = mfrc522.uid.uidByte[i];
  }

  // Check authorization
  if (isAuthorized(readUID)) {
	accessGranted();
	logToCloud(uidToString(readUID), "BERHASIL");
	playSuccessSound();
	failedAttempts = 0;
  } else {
	failedAttempts++;
	if (failedAttempts >= 3) {
  	playWarningSound();
	logToCloud(uidToString(readUID), "GAGAL");
	accessDenied();
  	failedAttempts = 0;
  	return;
	}
	playErrorSound();
	accessDenied();
	logToCloud(uidToString(readUID), "GAGAL");
   
  }

  // Update last scan time
  lastScanTime = millis();

  // Finish reading session
  mfrc522.PICC_HaltA();
  delay(500);
 
  // Allow reading again
  isProcessing = false;
}

// --- Helper Functions ---

void displayReadyMessage() {
  lcd.setCursor(0, 0);
  lcd.print("TEMPELKAN KARTU");
  lcd.setCursor(0, 1);
  lcd.print("            	"); // Clear line
}

void displayProcessing() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SEDANG PROSES");
 
  // Animated dots
  for (int i = 0; i < 3; i++) {
	lcd.print(".");
	delay(300);
  }
}

bool isAuthorized(byte* uid) {
  for (int i = 0; i < 4; i++) {
	if (uid[i] != validUID[i]) return false;
  }
  return true;
}

void accessGranted() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AKSES BERHASIL");
  lcd.setCursor(0, 1);
  lcd.print("SELAMAT DATANG!");
  digitalWrite(RELAY_PIN, LOW); // Unlock
  delay(5000);
  digitalWrite(RELAY_PIN, HIGH); // Lock
 }

void accessDenied() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AKSES DITOLAK");
  delay(2000);
}

void logToCloud(String uid, String status) {
  if (WiFi.status() != WL_CONNECTED) {
	displayNetworkError();
	return;
  }

  // Show sending status
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MENGIRIM DATA");
  for (int i = 0; i < 3; i++) {
	lcd.print(".");
	delay(300);
  }

  HTTPClient http;
  String url = String(scriptURL) + “?status=" + status;
  http.begin(url);
  int httpCode = http.GET();
  http.end();

  // Show result
  lcd.clear();
  lcd.setCursor(0, 0);
  if (httpCode > 0) {
	lcd.print("DATA TERKIRIM");
  } else {
	lcd.print("PENGIRIMAN DATA");
	lcd.setCursor(0, 1);
	lcd.print("GAGAL");
  }
  delay(1000);
}

void displayNetworkError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("NETWORK ERROR");
  lcd.setCursor(0, 1);
  lcd.print("CEK KONEKSI");
  delay(2000);
}

void playSuccessSound() {
  tone(BUZZER_PIN, 1000, 500);
  delay(600);
  tone(BUZZER_PIN, 1500, 500);
}

void playErrorSound() {
  tone(BUZZER_PIN, 1000, 500);
}

void playWarningSound() {
  for (int i = 0; i < 3; i++) {
	tone(BUZZER_PIN, 1000, 300);
	delay(400);
  }
}
