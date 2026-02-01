/**
 * CrowBot Bolt MQTT Controller
 * =============================
 * Part of the Agatha-BOT project
 * 
 * This firmware allows the CrowBot Bolt to receive movement commands
 * via MQTT from Home Assistant, enabling voice control of the robot.
 * 
 * Hardware: CrowBot Bolt (ESP32-WROVER-B based)
 * 
 * Setup:
 * 1. Update WiFi credentials below
 * 2. Update MQTT server IP (your Home Assistant)
 * 3. Flash using Arduino IDE or PlatformIO
 * 
 * MQTT Topics:
 * - crowbot/command (subscribe) - Receives movement commands
 * - crowbot/status (publish) - Reports current status
 * 
 * Commands: forward, backward, left, right, stop, dance, beep
 */

#include <WiFi.h>
#include <PubSubClient.h>

// =============================================================================
// CONFIGURATION - UPDATE THESE VALUES!
// =============================================================================

// WiFi Settings
const char* WIFI_SSID = "YOUR_WIFI_SSID";           // Your WiFi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";    // Your WiFi password

// MQTT Settings (Home Assistant)
const char* MQTT_SERVER = "YOUR_HOME_ASSISTANT_IP";  // e.g., "192.168.1.100"
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";       // Leave empty if no auth, or set in Mosquitto
const char* MQTT_PASSWORD = "";   // Leave empty if no auth

// MQTT Topics
const char* TOPIC_COMMAND = "crowbot/command";  // Commands from Home Assistant
const char* TOPIC_STATUS = "crowbot/status";    // Status updates to Home Assistant

// Movement timing (milliseconds)
const int MOVE_DURATION = 1000;   // How long to move for forward/backward
const int TURN_DURATION = 500;    // How long to turn for left/right
const int MOTOR_SPEED = 200;      // PWM value 0-255

// =============================================================================
// HARDWARE PIN DEFINITIONS - CrowBot Bolt
// =============================================================================

// Motor pins (DC motors controlled via H-bridge)
#define MOTOR_LEFT_MINUS   12   // Left motor negative
#define MOTOR_LEFT_PLUS    13   // Left motor positive
#define MOTOR_RIGHT_MINUS  14   // Right motor negative
#define MOTOR_RIGHT_PLUS   15   // Right motor positive

// Accessories
#define LED_PIN            25   // RGB LED (WS2812)
#define BUZZER_PIN         33   // Piezo buzzer
#define ULTRASONIC_TRIG    27   // Ultrasonic sensor trigger (optional)
#define ULTRASONIC_ECHO    26   // Ultrasonic sensor echo (optional)

// PWM Channels for motor control
#define PWM_CHANNEL_LM     0    // Left motor minus
#define PWM_CHANNEL_LP     1    // Left motor plus
#define PWM_CHANNEL_RM     2    // Right motor minus
#define PWM_CHANNEL_RP     3    // Right motor plus

// =============================================================================
// GLOBAL OBJECTS
// =============================================================================

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// State tracking
bool isMoving = false;
String currentCommand = "idle";
unsigned long lastStatusUpdate = 0;
const unsigned long STATUS_INTERVAL = 10000;  // Send status every 10 seconds

// =============================================================================
// FUNCTION DECLARATIONS
// =============================================================================

void setupMotors();
void setupWiFi();
void setupMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();
void publishStatus(const char* status);
void setMotors(int leftMinus, int leftPlus, int rightMinus, int rightPlus);
void motorForward();
void motorBackward();
void motorLeft();
void motorRight();
void motorStop();
void danceSequence();
void beep(int duration);
void beepPattern(int count, int duration, int pause);

// =============================================================================
// SETUP
// =============================================================================

void setup() {
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println();
  Serial.println("========================================");
  Serial.println("  CrowBot Bolt MQTT Controller");
  Serial.println("  Part of Agatha's BOT Project");
  Serial.println("========================================");
  Serial.println();
  
  // Initialize hardware
  Serial.println("[INIT] Setting up motors...");
  setupMotors();
  
  Serial.println("[INIT] Setting up buzzer and LED...");
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Startup beep
  beep(100);
  
  // Connect to WiFi
  Serial.println("[INIT] Connecting to WiFi...");
  setupWiFi();
  
  // Setup MQTT
  Serial.println("[INIT] Setting up MQTT...");
  setupMQTT();
  
  // Ready signal
  Serial.println("[INIT] CrowBot ready!");
  Serial.println();
  beepPattern(2, 100, 100);  // Two short beeps = ready
  
  publishStatus("online");
}

// =============================================================================
// MAIN LOOP
// =============================================================================

void loop() {
  // Ensure MQTT connection
  if (!mqtt.connected()) {
    reconnectMQTT();
  }
  mqtt.loop();
  
  // Periodic status update
  if (millis() - lastStatusUpdate > STATUS_INTERVAL) {
    publishStatus(currentCommand.c_str());
    lastStatusUpdate = millis();
  }
}

// =============================================================================
// MOTOR SETUP AND CONTROL
// =============================================================================

void setupMotors() {
  // Configure PWM channels
  // Parameters: channel, frequency (Hz), resolution (bits)
  ledcSetup(PWM_CHANNEL_LM, 5000, 8);  // 5kHz, 8-bit resolution (0-255)
  ledcSetup(PWM_CHANNEL_LP, 5000, 8);
  ledcSetup(PWM_CHANNEL_RM, 5000, 8);
  ledcSetup(PWM_CHANNEL_RP, 5000, 8);
  
  // Attach PWM channels to GPIO pins
  ledcAttachPin(MOTOR_LEFT_MINUS, PWM_CHANNEL_LM);
  ledcAttachPin(MOTOR_LEFT_PLUS, PWM_CHANNEL_LP);
  ledcAttachPin(MOTOR_RIGHT_MINUS, PWM_CHANNEL_RM);
  ledcAttachPin(MOTOR_RIGHT_PLUS, PWM_CHANNEL_RP);
  
  // Start with motors stopped
  motorStop();
  Serial.println("[MOTORS] Initialized and stopped");
}

void setMotors(int leftMinus, int leftPlus, int rightMinus, int rightPlus) {
  ledcWrite(PWM_CHANNEL_LM, leftMinus);
  ledcWrite(PWM_CHANNEL_LP, leftPlus);
  ledcWrite(PWM_CHANNEL_RM, rightMinus);
  ledcWrite(PWM_CHANNEL_RP, rightPlus);
}

void motorForward() {
  Serial.println("[MOTOR] Moving forward");
  isMoving = true;
  setMotors(0, MOTOR_SPEED, 0, MOTOR_SPEED);
  delay(MOVE_DURATION);
  motorStop();
}

void motorBackward() {
  Serial.println("[MOTOR] Moving backward");
  isMoving = true;
  setMotors(MOTOR_SPEED, 0, MOTOR_SPEED, 0);
  delay(MOVE_DURATION);
  motorStop();
}

void motorLeft() {
  Serial.println("[MOTOR] Turning left");
  isMoving = true;
  // Left motor backward, right motor forward
  setMotors(MOTOR_SPEED/2, 0, 0, MOTOR_SPEED);
  delay(TURN_DURATION);
  motorStop();
}

void motorRight() {
  Serial.println("[MOTOR] Turning right");
  isMoving = true;
  // Left motor forward, right motor backward
  setMotors(0, MOTOR_SPEED, MOTOR_SPEED/2, 0);
  delay(TURN_DURATION);
  motorStop();
}

void motorStop() {
  Serial.println("[MOTOR] Stopping");
  isMoving = false;
  setMotors(0, 0, 0, 0);
}

void danceSequence() {
  Serial.println("[MOTOR] Starting dance sequence!");
  isMoving = true;
  beep(100);
  
  // Spin right
  Serial.println("[DANCE] Spin right");
  setMotors(0, MOTOR_SPEED, MOTOR_SPEED, 0);
  delay(400);
  
  // Spin left
  Serial.println("[DANCE] Spin left");
  setMotors(MOTOR_SPEED, 0, 0, MOTOR_SPEED);
  delay(400);
  
  // Forward wiggle
  Serial.println("[DANCE] Forward");
  setMotors(0, MOTOR_SPEED, 0, MOTOR_SPEED);
  delay(200);
  
  // Backward wiggle
  Serial.println("[DANCE] Backward");
  setMotors(MOTOR_SPEED, 0, MOTOR_SPEED, 0);
  delay(200);
  
  // Side to side wiggle
  Serial.println("[DANCE] Wiggle!");
  for (int i = 0; i < 4; i++) {
    setMotors(0, MOTOR_SPEED * 0.6, 0, MOTOR_SPEED);
    delay(150);
    setMotors(0, MOTOR_SPEED, 0, MOTOR_SPEED * 0.6);
    delay(150);
  }
  
  // Final spin
  Serial.println("[DANCE] Final spin!");
  setMotors(0, MOTOR_SPEED, MOTOR_SPEED, 0);
  delay(600);
  
  motorStop();
  beepPattern(3, 50, 50);  // Celebration beeps!
  Serial.println("[DANCE] Dance complete!");
}

// =============================================================================
// NETWORK AND MQTT
// =============================================================================

void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("[WIFI] Connecting to ");
  Serial.print(WIFI_SSID);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected!");
    Serial.print("[WIFI] IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WIFI] Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println(" FAILED!");
    Serial.println("[WIFI] Will retry in main loop...");
  }
}

void setupMQTT() {
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);  // Increase buffer for longer messages
}

void reconnectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("[MQTT] Connecting to ");
    Serial.print(MQTT_SERVER);
    Serial.print("...");
    
    // Create a unique client ID
    String clientId = "CrowBot-";
    clientId += String(random(0xffff), HEX);
    
    bool connected = false;
    if (strlen(MQTT_USER) > 0) {
      connected = mqtt.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD);
    } else {
      connected = mqtt.connect(clientId.c_str());
    }
    
    if (connected) {
      Serial.println(" Connected!");
      
      // Subscribe to command topic
      mqtt.subscribe(TOPIC_COMMAND);
      Serial.print("[MQTT] Subscribed to: ");
      Serial.println(TOPIC_COMMAND);
      
      // Announce we're online
      publishStatus("online");
    } else {
      Serial.print(" Failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String command = "";
  for (unsigned int i = 0; i < length; i++) {
    command += (char)payload[i];
  }
  command.toLowerCase();  // Normalize to lowercase
  
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.print("[MQTT] Received command: '");
  Serial.print(command);
  Serial.println("'");
  
  // Process commands
  if (command == "forward" || command == "go" || command == "ahead") {
    currentCommand = "forward";
    motorForward();
    publishStatus("moved_forward");
  }
  else if (command == "backward" || command == "back" || command == "reverse") {
    currentCommand = "backward";
    motorBackward();
    publishStatus("moved_backward");
  }
  else if (command == "left" || command == "turn_left") {
    currentCommand = "left";
    motorLeft();
    publishStatus("turned_left");
  }
  else if (command == "right" || command == "turn_right") {
    currentCommand = "right";
    motorRight();
    publishStatus("turned_right");
  }
  else if (command == "stop" || command == "halt" || command == "freeze") {
    currentCommand = "stopped";
    motorStop();
    publishStatus("stopped");
  }
  else if (command == "dance" || command == "party") {
    currentCommand = "dancing";
    publishStatus("dancing");
    danceSequence();
    publishStatus("dance_complete");
  }
  else if (command == "beep" || command == "honk") {
    beep(200);
    publishStatus("beeped");
  }
  else if (command == "status" || command == "ping") {
    publishStatus("alive");
  }
  else {
    Serial.print("[MQTT] Unknown command: ");
    Serial.println(command);
    publishStatus("unknown_command");
  }
  
  currentCommand = "idle";
  Serial.println("----------------------------------------");
}

void publishStatus(const char* status) {
  if (mqtt.connected()) {
    mqtt.publish(TOPIC_STATUS, status);
    Serial.print("[MQTT] Published status: ");
    Serial.println(status);
  }
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

void beep(int duration) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration);
  digitalWrite(BUZZER_PIN, LOW);
}

void beepPattern(int count, int duration, int pause) {
  for (int i = 0; i < count; i++) {
    beep(duration);
    if (i < count - 1) {
      delay(pause);
    }
  }
}
