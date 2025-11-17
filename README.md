# Planty
## Plant monitoring with personality

> 🚧 THIS PROJECT IS IN THE EARLY STAGES OF DEVELOPMENT. 🚧
> 
> The information in this page is subject to change in the future. Check the project logs for progress updates

Planty is a device that monitors your plant's living conditions by means of measuring soil humidity and temperature as well as ambient humidity, temperature and light level. 
Every plant is different, and thus each one has its own humidity and temperature requirements. The user will input the type of plant being monitored and Planty will automatically
decide the best living conditions for it based on online sources. 

Every plant is different, and that comes with an interesting twist: every plant's personality is different too. The user will not respond to a boring, generic notification to water their
plants. Instead, it will receive a message from their plants to take care of them, and each one of them will do it in their own personal way. A Whatsapp message will be sent to the user when an update is required. If this turns out to not be possible 
due to technical limitations with Whatsapp's API, a simple app will be the means of communication with the user.

The measurement of each parameter is achieved with a specific sensor. All sensors are then combined with an ESP32 running software that combines these measurements and interfaces with the 
user.
| Part                                  | Description                             | Part number   |
|---------------------------------------|-----------------------------------------|---------------|
| Soil humidity sensor                  | Capacitive moisture sensor              | LM393         |
| Soil temperature sensor               | Thin film thermistor                    | MF5B          |
| Ambient temperature + humidity sensor | Combined thermistor and humidity sensor | AM2320        |
| Display                               | E-ink, 212x104 pixels                   | MH-ET LIVE    |
| MCU                                   | ESP32 with WiFi and BLE functionality   | ESP32-WROOM32 |
