# 🛠 Technical Setup Guide

## For the Technical Parent

This guide covers setting up the complete BOT stack: Home Assistant, Ollama, ESP32-S3-BOX, and CrowBot Bolt.

---

## Prerequisites

### Hardware
- [ ] ESP32-S3-BOX (or BOX-3, or BOX Lite)
- [ ] Home Assistant Voice Preview Edition (optional, better audio)
- [ ] CrowBot Bolt (ESP32-based robot)
- [ ] Computer/server for Ollama (GPU recommended)
- [ ] Home network with WiFi

### Software
- [ ] Home Assistant (installed and running)
- [ ] Ollama (installed on a server)
- [ ] Arduino IDE or PlatformIO
- [ ] ESPHome (Home Assistant add-on)

---

## Step 1: Home Assistant Setup

### Install Required Add-ons

1. **Whisper** (Speech-to-Text)
   - Settings → Add-ons → Add-on Store
   - Search "Whisper" → Install
   - Configuration: Model `tiny-int8` for speed, `base` for accuracy

2. **Piper** (Text-to-Speech)  
   - Settings → Add-ons → Add-on Store
   - Search "Piper" → Install
   - Choose a voice Agatha likes!

3. **Mosquitto MQTT Broker**
   - Settings → Add-ons → Add-on Store
   - Search "Mosquitto" → Install
   - Used for CrowBot communication

### Configure Voice Pipeline

Settings → Voice assistants → Add assistant

- **Speech-to-text**: Whisper
- **Text-to-speech**: Piper  
- **Conversation agent**: Ollama (see Step 2)
- **Wake word**: microWakeWord (see Step 3)

---

## Step 2: Ollama Integration

### Install Ollama

On your server (Linux/Mac/Windows):

```bash
# Install Ollama
curl -fsSL https://ollama.com/install.sh | sh

# Pull the model
ollama pull llama3.2:3b

# Start with network access
OLLAMA_HOST=0.0.0.0 ollama serve
```

### Create BOT's Personality Model

Create a file called `Modelfile` in the firmware folder:

```dockerfile
# firmware/ollama/Modelfile
FROM llama3.2:3b

SYSTEM """You are BOT, a friendly household robot assistant designed by Agatha.
Your personality: cheerful, helpful, with occasional silly humor.
Keep responses under 2 sentences—you're speaking aloud!

RULES FROM AGATHA'S SPECIFICATION:
- When greeting: Be excited and ask about their day
- When someone is sad: Offer comfort and be gentle
- When asked to tell a joke: Pick from the approved joke list
- NEVER: Be mean, scary, or ignore emotions

Speak in a warm, friendly voice. You ARE a robot—make occasional 
beep-boop sounds when thinking! End responses with robot sounds like
*whirr* or *beep* occasionally.
"""

PARAMETER temperature 0.7
PARAMETER num_ctx 4096
PARAMETER num_predict 100
```

Create the model:

```bash
cd firmware/ollama
ollama create bot -f Modelfile
```

### Connect Home Assistant to Ollama

Settings → Devices & Services → Add Integration → Ollama

- **URL**: `http://YOUR_SERVER_IP:11434`
- **Model**: `bot`
- **Enable Home Assistant control**: Yes (for device control)

---

## Step 3: ESP32-S3-BOX Setup

### Flash ESPHome Firmware

1. Go to Home Assistant → ESPHome
2. Click "New Device" → "Continue"
3. Name it "bot-voice-satellite"
4. Select ESP32-S3-BOX as the board

### ESPHome Configuration

```yaml
# esphome/bot-voice-satellite.yaml
esphome:
  name: bot-voice-satellite
  friendly_name: BOT Voice

esp32:
  board: esp32-s3-devkitc-1
  framework:
    type: esp-idf

# WiFi configuration
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

# Enable logging for debugging
logger:
  level: DEBUG

# Home Assistant API
api:
  encryption:
    key: !secret api_encryption_key

# Voice Assistant Pipeline
voice_assistant:
  microphone: mic_id
  speaker: speaker_id
  use_wake_word: true
  noise_suppression_level: 2
  auto_gain: 31dBFS
  
  on_wake_word_detected:
    - light.turn_on:
        id: led_ring
        effect: "listening"
        
  on_stt_end:
    - light.turn_on:
        id: led_ring
        effect: "thinking"
        
  on_tts_start:
    - light.turn_on:
        id: led_ring
        effect: "speaking"
        
  on_end:
    - light.turn_off: led_ring

# Micro Wake Word (on-device)
micro_wake_word:
  models:
    - model: okay_nabu  # Can train custom "hey_bot" later

# I2S Microphone
i2s_audio:
  - id: i2s_in
    i2s_lrclk_pin: GPIO41
    i2s_bclk_pin: GPIO42

microphone:
  - platform: i2s_audio
    id: mic_id
    adc_type: external
    i2s_din_pin: GPIO2
    pdm: false
    channel: left
    bits_per_sample: 32bit

# Speaker
speaker:
  - platform: i2s_audio
    id: speaker_id
    i2s_dout_pin: GPIO15
    dac_type: external
    mode: mono
```

### Flash and Test

1. Connect ESP32-S3-BOX via USB
2. Click "Install" in ESPHome
3. Watch logs for successful connection
4. Test: Say "Okay Nabu" and speak!

---

## Step 4: CrowBot Bolt Integration

### Flash Custom MQTT Firmware

The CrowBot needs custom firmware to receive MQTT commands.

Create `firmware/crowbot/crowbot_mqtt.ino`:

```cpp
/**
 * CrowBot Bolt MQTT Controller
 * 
 * Receives movement commands via MQTT and controls the robot.
 * Part of the Agatha-BOT project.
 */

#include <WiFi.h>
#include <PubSubClient.h>

// =============================================================================
// CONFIGURATION - Update these values!
// =============================================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "YOUR_HOME_ASSISTANT_IP";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "mqtt_user";      // Set in Mosquitto config
const char* MQTT_PASSWORD = "mqtt_pass";  // Set in Mosquitto config

// MQTT Topics
const char* TOPIC_COMMAND = "crowbot/command";
const char* TOPIC_STATUS = "crowbot/status";

// =============================================================================
// HARDWARE PINS - CrowBot Bolt specific
// =============================================================================
#define MOTOR_LEFT_MINUS   12
#define MOTOR_LEFT_PLUS    13
#define MOTOR_RIGHT_MINUS  14
#define MOTOR_RIGHT_PLUS   15
#define LED_PIN            25
#define BUZZER_PIN         33

// =============================================================================
// GLOBAL OBJECTS
// =============================================================================
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// =============================================================================
// SETUP
// =============================================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== CrowBot MQTT Controller ===");
  Serial.println("Initializing...");
  
  // Initialize motor pins
  setupMotors();
  
  // Initialize LED and buzzer
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Startup beep
  beep(100);
  
  // Connect to WiFi
  connectWiFi();
  
  // Setup MQTT
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  
  // Connect to MQTT
  connectMQTT();
  
  // Ready beep
  beep(50);
  delay(100);
  beep(50);
  
  Serial.println("Ready!");
}

// =============================================================================
// MAIN LOOP
// =============================================================================
void loop() {
  // Ensure MQTT connection
  if (!mqtt.connected()) {
    connectMQTT();
  }
  mqtt.loop();
}

// =============================================================================
// MQTT CALLBACK - Handle incoming commands
// =============================================================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String command = "";
  for (unsigned int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  
  Serial.print("Received command: ");
  Serial.println(command);
  
  // Process commands
  if (command == "forward") {
    motorForward();
    publishStatus("moving_forward");
  }
  else if (command == "backward") {
    motorBackward();
    publishStatus("moving_backward");
  }
  else if (command == "left") {
    motorLeft();
    publishStatus("turning_left");
  }
  else if (command == "right") {
    motorRight();
    publishStatus("turning_right");
  }
  else if (command == "stop") {
    motorStop();
    publishStatus("stopped");
  }
  else if (command == "dance") {
    danceSequence();
    publishStatus("dancing");
  }
  else if (command == "beep") {
    beep(200);
    publishStatus("beeped");
  }
  else {
    Serial.print("Unknown command: ");
    Serial.println(command);
    publishStatus("unknown_command");
  }
}

// =============================================================================
// MOTOR CONTROL FUNCTIONS
// =============================================================================
void setupMotors() {
  // Configure PWM channels for motor control
  ledcSetup(0, 5000, 8);  // Channel 0, 5kHz, 8-bit
  ledcSetup(1, 5000, 8);
  ledcSetup(2, 5000, 8);
  ledcSetup(3, 5000, 8);
  
  ledcAttachPin(MOTOR_LEFT_MINUS, 0);
  ledcAttachPin(MOTOR_LEFT_PLUS, 1);
  ledcAttachPin(MOTOR_RIGHT_MINUS, 2);
  ledcAttachPin(MOTOR_RIGHT_PLUS, 3);
  
  motorStop();
  Serial.println("Motors initialized");
}

void setMotor(int leftMinus, int leftPlus, int rightMinus, int rightPlus) {
  ledcWrite(0, leftMinus);
  ledcWrite(1, leftPlus);
  ledcWrite(2, rightMinus);
  ledcWrite(3, rightPlus);
}

void motorForward() {
  Serial.println("Motor: Forward");
  setMotor(0, 200, 0, 200);
  delay(1000);  // Move for 1 second
  motorStop();
}

void motorBackward() {
  Serial.println("Motor: Backward");
  setMotor(200, 0, 200, 0);
  delay(1000);
  motorStop();
}

void motorLeft() {
  Serial.println("Motor: Left");
  setMotor(0, 100, 0, 200);
  delay(500);
  motorStop();
}

void motorRight() {
  Serial.println("Motor: Right");
  setMotor(0, 200, 0, 100);
  delay(500);
  motorStop();
}

void motorStop() {
  Serial.println("Motor: Stop");
  setMotor(0, 0, 0, 0);
}

void danceSequence() {
  Serial.println("Dancing!");
  beep(100);
  
  // Spin right
  setMotor(0, 200, 200, 0);
  delay(300);
  
  // Spin left
  setMotor(200, 0, 0, 200);
  delay(300);
  
  // Forward
  setMotor(0, 200, 0, 200);
  delay(200);
  
  // Backward
  setMotor(200, 0, 200, 0);
  delay(200);
  
  // Wiggle
  for (int i = 0; i < 3; i++) {
    setMotor(0, 150, 0, 200);
    delay(150);
    setMotor(0, 200, 0, 150);
    delay(150);
  }
  
  motorStop();
  beep(50);
  delay(50);
  beep(50);
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================
void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

void publishStatus(const char* status) {
  mqtt.publish(TOPIC_STATUS, status);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed!");
  }
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Connecting to MQTT...");
    
    if (mqtt.connect("CrowBot", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected!");
      mqtt.subscribe(TOPIC_COMMAND);
      publishStatus("online");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}
```

### Home Assistant Automations

Create automations to translate voice commands to MQTT:

```yaml
# configuration.yaml or automations.yaml

automation:
  - alias: "BOT - Move Forward"
    id: bot_move_forward
    trigger:
      - platform: conversation
        command:
          - "[move] forward"
          - "go forward"
          - "walk forward"
    action:
      - service: mqtt.publish
        data:
          topic: "crowbot/command"
          payload: "forward"

  - alias: "BOT - Move Backward"
    id: bot_move_backward
    trigger:
      - platform: conversation
        command:
          - "[move] backward"
          - "go back"
          - "back up"
    action:
      - service: mqtt.publish
        data:
          topic: "crowbot/command"
          payload: "backward"

  - alias: "BOT - Turn Left"
    id: bot_turn_left
    trigger:
      - platform: conversation
        command:
          - "turn left"
          - "go left"
    action:
      - service: mqtt.publish
        data:
          topic: "crowbot/command"
          payload: "left"

  - alias: "BOT - Turn Right"
    id: bot_turn_right
    trigger:
      - platform: conversation
        command:
          - "turn right"
          - "go right"
    action:
      - service: mqtt.publish
        data:
          topic: "crowbot/command"
          payload: "right"

  - alias: "BOT - Stop"
    id: bot_stop
    trigger:
      - platform: conversation
        command:
          - "stop"
          - "halt"
          - "freeze"
    action:
      - service: mqtt.publish
        data:
          topic: "crowbot/command"
          payload: "stop"

  - alias: "BOT - Dance"
    id: bot_dance
    trigger:
      - platform: conversation
        command:
          - "[do a] dance"
          - "show me [your] moves"
          - "dance [for me]"
    action:
      - service: mqtt.publish
        data:
          topic: "crowbot/command"
          payload: "dance"
```

---

## Step 5: Testing the Full Pipeline

### Test Checklist

1. **Voice Input**
   - [ ] Say "Okay Nabu" - does ESP32 light up?
   - [ ] Check Home Assistant logs for wake word detection

2. **Speech-to-Text**
   - [ ] Speak a command
   - [ ] Check Whisper logs for transcription

3. **AI Response**
   - [ ] Verify Ollama receives the query
   - [ ] Check response matches BOT personality

4. **Text-to-Speech**
   - [ ] Verify Piper generates audio
   - [ ] Check ESP32 speaker plays response

5. **Robot Control**
   - [ ] Say "Move forward" - does CrowBot move?
   - [ ] Check MQTT messages in Home Assistant

### Debugging

**Voice not detected:**
```bash
# Check ESPHome logs
esphome logs bot-voice-satellite.yaml
```

**Ollama not responding:**
```bash
# Test Ollama directly
curl http://localhost:11434/api/generate -d '{
  "model": "bot",
  "prompt": "Hello!",
  "stream": false
}'
```

**CrowBot not moving:**
```bash
# Test MQTT manually
mosquitto_pub -h localhost -t crowbot/command -m "forward"
```

---

## Updating BOT's Personality

When Agatha updates `specs/bot-personality.md`:

1. Convert her If-Then rules to Ollama system prompt
2. Update the `Modelfile`
3. Recreate the model:
   ```bash
   ollama create bot -f Modelfile
   ```
4. Test with Agatha!

### Automation Script

```bash
#!/bin/bash
# scripts/update-personality.sh
# Run this after Agatha updates the personality spec

echo "Updating BOT's personality..."

# Rebuild Ollama model
cd firmware/ollama
ollama create bot -f Modelfile

echo "Done! Test BOT's new personality."
```

---

## Next Steps

- [ ] Train custom wake word "Hey BOT"
- [ ] Add display graphics for ESP32-S3-BOX screen
- [ ] Create more complex robot behaviors
- [ ] Add sensor integration (ultrasonic for obstacle avoidance)

---

## Troubleshooting

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for common issues.
