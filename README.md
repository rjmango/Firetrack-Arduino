# Firetrack-Arduino
Repo for the arduino codespace for firetrack

# Requirements
## Step 1: Install the ESP8266 Board Manager
By default, Arduino only knows about its own boards (Uno, Mega, etc.). You need to add the ESP8266 "Core."

1. Open Arduino IDE.
2. Go to File > Preferences.
3. Look for the field "Additional Boards Manager URLs".
4. Paste this URL into the box: http://arduino.esp8266.com/stable/package_esp8266com_index.json (If you already have a URL there, separate them with a comma).
5. Click OK.

## Step 2: Download the Board Files
1. Go to Tools > Board > Boards Manager...
2. In the search bar, type ESP8266.
3. Find the one titled esp8266 by ESP8266 Community.
4. Click Install. This may take a minute as it downloads the compiler and tools.

## Step 3: Install the Libraries
1. TinyGPSPlus (by Mikal Hart)
2. ArduinoJson (by Benoit Blanchon)

# Hardware Setup
1. Power nodeMCU with microUSB
2. Connect Vcc of GPS Module to Vin
3. Connect Tx pin of GPS Module to D6 Pin of NodeMCU
4. Connect Rx pin of GPS Module to D5 Pin of NodeMCU
5. Connect Antenna to GPS Module
