# DESCRIPTION 

This project is a Smart Digital Door Lock System using RFID technology for secure access control. The system is powered by an ESP32 and integrates multiple components such as an RFID reader, LCD display, buzzer, relay, and solenoid lock. It also supports real-time access logging to a cloud server (Google Sheets), allowing users to monitor door activity remotely and enhance security.

# Tools & Materials

1. ESP32 (ESP32 Devkit V1)
2. RFID Reader (MFRC522)
3. RFID Cards/Tags
4. Solenoid Door Lock (12V)
5. Relay Module (5V, 1 Channel)
6. LCD 16x2 with I2C (0x27)
7. Buzzer (Active 5V)
8. Power Supply 5V (USB / Powerbank)
9. Power Supply 12V (Battery)
10. Jumper Wires & Breadboard

# How It Works
1. The RFID reader scans the card and sends its UID to the ESP32.
2. The ESP32 verifies the UID with stored authorized data.
3. If the card is valid:

   a. The buzzer sounds a long beep.
   
   b. The relay activates and powers the solenoid lock (door opens).
   
   c. The LCD displays “Access Granted”.
   
   d. Access data is sent to Google Sheets for logging.
5. If the card is invalid:
   
   a. The buzzer gives a short beep.
   
   b. After 3 failed attempts, it beeps three times as a warning.
   
   c. The LCD displays “Access Denied”.
7. The system continuously logs access activity to the cloud in real-time.

# Steps
1. Prepare all components including ESP32, RFID reader, relay, solenoid lock, LCD, and buzzer.
2. Connect the RFID module to the ESP32 for UID reading.
3. Connect the relay module to control the 12V solenoid lock.
4. Connect the LCD (I2C) to display system status.
5. Connect the buzzer for audio feedback.
6. Provide power: 5V for ESP32 and components, 12V for solenoid lock.
7. Upload the program to the ESP32.
8. Configure authorized RFID UIDs in the code.
9. Integrate the system with Google Sheets for access logging.
10. Test the system using valid and invalid RFID cards.

# Hardware Configuration
1. ESP32 Pin Connections

| Pin Component | Connected To (ESP32) |
| ------------- | -------------------- |
| GPIO 2        | RST (RFID)           |
| GPIO 19       | MISO (RFID)          |
| GPIO 23       | MOSI (RFID)          |
| GPIO 18       | SCK (RFID)           |
| GPIO 5        | SS/SDA (RFID)        |
| GPIO 13       | Buzzer (+)           |
| GPIO 21       | SDA (LCD I2C)        |
| GPIO 22       | SCL (LCD I2C)        |
| GPIO 4        | Relay IN             |
| GND           | All GND components   |
| VIN / 5V      | Breadboard Power     |
| 3.3V          | Breadboard Power     |

2. RFID RC522 Connections

| Pin RFID | Connected To    |
| -------- | --------------- |
| SDA (SS) | GPIO 5 (ESP32)  |
| SCK      | GPIO 18 (ESP32) |
| MOSI     | GPIO 23 (ESP32) |
| MISO     | GPIO 19 (ESP32) |
| RST      | GPIO 2 (ESP32)  |
| VCC      | 3.3V            |
| GND      | GND             |


3. LCD I2C 16x2 Connections

| Pin LCD | Connected To    |
| ------- | --------------- |
| SDA     | GPIO 21 (ESP32) |
| SCL     | GPIO 22 (ESP32) |
| VCC     | 5V              |
| GND     | GND             |


4. Relay Module (5V)

| Pin Relay | Connected To      |
| --------- | ----------------- |
| IN        | GPIO 4 / GPIO 15* |
| VCC       | 5V                |
| GND       | GND               |
| COM       | +12V Supply       |
| NC        | + Solenoid        |


5. Solenoid Lock

| Pin     | Connected To   |
| ------- | -------------- |
| VCC (+) | NC Relay       |
| GND     | GND 12V Supply |


6. Buzzer (Active)

| Pin          | Connected To |
| ------------ | ------------ |
| Positive (+) | GPIO 13      |
| Negative (-) | GND          |

# Code
Code Structure : 
1. Arduino : smart-digital-doorlock/code/arduino/smartdoor.ino
