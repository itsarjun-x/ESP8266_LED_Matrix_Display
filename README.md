# ESP8266 LED Matrix Display

A multifunctional LED matrix display built using **NodeMCU ESP8266** and **FastLED**.  
This project includes three modes — a **real-time clock**, **Snake game (autoplay)**, and **Flappy Bird (autoplay)** — all running on a **30×10 WS2812B LED matrix**.

---

## Overview

This project demonstrates how to use the ESP8266 microcontroller with an addressable LED matrix to create real-time visual displays and simple auto-play animations.  
It includes:

- Real-Time Clock (via NTP)
- Snake Game (auto-play)
- Flappy Bird (auto-play)

Each mode can be uploaded individually to the ESP8266.

---

## ⚙️ Features

✅ Real-time clock using NTPClient and WiFi  
✅ Smooth FastLED animations  
✅ Custom 12-hour time display  
✅ Auto-play Snake and Flappy Bird games  
✅ Built for a 30×10 WS2812B LED matrix  

---

## 🔌 Hardware Used

| Component | Description |
|------------|-------------|
| **Microcontroller** | NodeMCU ESP8266 |
| **LED Matrix** | 30×10 WS2812B addressable RGB LEDs |
| **Power Supply** | 5V / 10A |
| **Resistor** | 330Ω between data pin and LED DIN |
| **Capacitor** | 1000µF 25V across 5V and GND |
| **Logic Level Shifter** | 3.3V → 5V for data signal stability |

---

## 🧩 Libraries Used

| Library | Description |
|----------|-------------|
| [FastLED](https://github.com/FastLED/FastLED) | Controls WS2812B LEDs |
| [ESP8266WiFi](https://github.com/esp8266/Arduino) | Connects ESP8266 to WiFi |
| [WiFiUdp](https://github.com/esp8266/Arduino) | Supports NTP communication |
| [NTPClient](https://github.com/arduino-libraries/NTPClient) | Gets time from internet servers |

> Only the **Clock_Display.ino** uses WiFi/NTP;  
> Snake and Flappy Bird use **FastLED** only.

---

## 🖼️ Circuit Diagram

![Circuit Diagram](circuit/esp8266_led_matrix_circuit.png)

---

## 🚀 How to Run

1. Open **Arduino IDE**  
2. Select **NodeMCU 1.0 (ESP-12E Module)** from Tools → Board  
3. Install all required libraries listed in `libraries.txt`  
4. Open one of the `.ino` files from `/code`  
5. Update your WiFi credentials (for Clock code only)  
6. Upload and power the circuit  
7. Enjoy the LED animations 🎉


---

## 🧠 About

This project combines **IoT** and **embedded programming** concepts using the ESP8266 platform.  
It’s part of my learning journey in **electronics and coding** — exploring how hardware and software interact.

---

## 📜 License

This project is open-source under the **MIT License**.  
Feel free to use or modify it for learning and experimentation.

