# Planty
## Plant monitoring with personality

> ### 🚧 **Project Status: In-Progress**
> **THIS PROJECT IS IN THE EARLY STAGES OF DEVELOPMENT**. The information on this page is subject to change. Check the project logs for progress updates.

![Early breadboard prototype](./docs/images/251117-early_breadboard_prototype.png)

## The Idea
Standard plant monitors are boring. Users get generic notifications and learn to ignore them.

*Planty* solves this by giving each plant a unique personality. Instead of a dry notification, your plant will personally message you for care (e.g., "I'm so thirsty!" or "A bit too bright in here for me, don't you think?").

This project also aims to be "smart." The user inputs the plant type (e.g., "Monstera Deliciosa"), and Planty will fetch its ideal living conditions from online sources to create a custom care profile.

## Core Features
- **Comprehensive Monitoring**: Measures soil humidity, soil temperature, ambient humidity, ambient temperature, and light levels.

- **"Smart" Plant Profiles**: Automatically determines a plant's ideal conditions based on its type.

- **"Personality Engine"**: Delivers care notifications as personalized messages from the plant itself.

- **User Alerts**: Sends a WhatsApp message (first choice) or an app notification (backup plan) when the plant needs care.

- **Low-Power Display**: An E-ink display shows at-a-glance status.

## Hardware
| Part                                  | Description                             | Part number   |
|---------------------------------------|-----------------------------------------|---------------|
| Soil humidity sensor                  | Capacitive moisture sensor              | LM393         |
| Soil temperature sensor               | Thin film thermistor                    | MF5B          |
| Ambient temperature + humidity sensor | Combined thermistor and humidity sensor | AM2320        |
| Display                               | E-ink, 212x104 pixels                   | MH-ET LIVE    |
| MCU                                   | ESP32 with WiFi and BLE functionality   | ESP32-WROOM32 |
| Power                                 | LiPo battery and charger module         | 104050        |

## Project Roadmap
At this early stage, the project roadmap is as follows:
- [x] Initial hardware selection and parts list

- [ ] Breadboard prototyping and sensor validation

- [ ] Write firmware to read from all sensors

- [ ] Research and integrate with a plant database API

- [ ] Research and integrate with WhatsApp API (or begin app development)

- [ ] Design and 3D print an enclosure

- [ ] PCB design V1