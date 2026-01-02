# 📟 DeskPet – Interactive ESP32 Desktop Companion

## 📌 Project Overview

**DeskPet** is an interactive **desktop companion device** built using an **ESP32**, a **0.96” monochrome OLED display**, capacitive **touch input**, and **Bluetooth Low Energy (BLE)** communication with a mobile phone.
It combines **expressive animated eyes**, **real-time notifications**, **time & weather display**, and a **timer**, creating a playful yet functional smart desk gadget.

The core idea is to humanize notifications and system information by presenting them through **moods, animations, and icons**, rather than plain text alone.

This is a demo video of the project: 
https://youtu.be/UlQ4MDCcUWU
---
![WhatsApp Image 2026-01-03 at 12 28 17 AM](https://github.com/user-attachments/assets/84f75fbc-2b91-4cc8-90ab-520e9f303cc1)

## 🧠 Key Features

### 👀 Animated Personality (RoboEyes)

* Uses animated eyes to express different **emotional states**:

  * **Default** – normal idle behavior
  * **Happy** – triggered by touch or long-press
  * **Confused** – shown when a notification arrives
  * **Tired** – enters automatically after inactivity
* Natural behaviors like:

  * Blinking
  * Subtle eye movements
  * Idle animations
* Eye animations run independently from UI screens for smooth interaction.

---

### 🖐 Touch Interaction

* **Short press** → Cycle between screens
* **Long press** → Trigger happy animation
* **Touch while sleepy** → Wakes DeskPet with a laugh animation
* After **30 seconds of inactivity**, DeskPet automatically enters a **tired/sleepy mode**.

---

### 📱 Bluetooth Notifications

DeskPet receives notifications from a smartphone via **BLE**, parses the data, and displays them visually.

Supported notification types:

* **WhatsApp**
* **Instagram**
* **Spotify**
* **Settings**
* **Other apps (generic notification icon)**

Each notification screen includes:

* App-specific **monochrome icon**
* Friendly app name (e.g., `com.whatsapp` → `WhatsApp`)
* Notification title text
* Confused eye animation when the notification arrives

---

### 🕒 Time & Weather Screen

* Displays:

  * Current **time** (HH:MM format)
  * **Weather information** (e.g., “Clear 28°C”)
* Icons used:

  * Clock icon beside time
  * Weather icon beside weather text
  * Bluetooth icon indicating active phone connection
* Updated dynamically via BLE from the mobile app.

---

### ⏱ Timer Screen

* Countdown timer synced from the phone
* Shows:

  * Remaining time in **MM:SS**
  * Timer icon beside the countdown
* Updates every second in real time.

---

## 🖥 Screen Modes Summary

| Screen         | Description                                |
| -------------- | ------------------------------------------ |
| Eyes Screen    | Animated eyes with moods and expressions   |
| Time & Weather | Time, weather, Bluetooth status with icons |
| Notifications  | App icon + app name + notification text    |
| Timer          | Countdown timer with timer icon            |

---

## ⚙️ Hardware Used

* **ESP32 Dev Module**
* **0.96” OLED Display (128×64, I²C, monochrome)**
* **Capacitive Touch Sensor**
* **USB power supply**

---

## 🧩 Software & Libraries

* **Arduino IDE**
* **Adafruit SSD1306 & GFX** – OLED rendering
* **FluxGarage RoboEyes** – animated eyes
* **ArduinoJson** – notification data parsing
* **ESP32 BLE Libraries** – Bluetooth communication

---

# 📱 Mobile App Description (Companion App)

## 📲 Purpose of the App

The **DeskPet Mobile App** acts as a **BLE controller and data provider** for the DeskPet device.
It bridges the smartphone and the ESP32, allowing DeskPet to mirror important information and notifications in real time.

---

## 🔔 Notification Handling

* Captures system notifications from the phone
* Extracts:

  * App package name
  * Notification title/content
* Sends this data to DeskPet over BLE
* Enables app-based icon recognition on DeskPet (WhatsApp, Instagram, Spotify, etc.)

---

## 🕒 Time & Weather Sync

* Periodically sends:

  * Current phone time
  * Weather data (via phone’s weather API)
* Keeps DeskPet always up to date without RTC hardware.

---

## ⏱ Timer Control

* Allows the user to:

  * Start a timer
  * Set duration in seconds or minutes
* Timer value is sent to DeskPet and displayed live.

---

## 🔗 Bluetooth Communication

* Uses **BLE characteristics** for:

  * Notifications
  * Time & weather updates
  * Timer values
* Low power consumption
* Automatic reconnection support

---

## 🎯 Use Cases

* Desktop notification companion
* Pomodoro / focus timer
* Ambient time & weather display
* Fun interactive desk gadget
* Educational project demonstrating:

  * BLE
  * Embedded graphics
  * UI state machines
  * Human–computer interaction

---

## 🚀 Conclusion

**DeskPet** transforms ordinary notifications into an **engaging, expressive experience**.
By combining embedded systems, Bluetooth communication, animation, and UI design, the project demonstrates how low-cost hardware can be turned into a **delightful smart device**.


