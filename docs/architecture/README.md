# Building BOT: A Voice-AI Robot Architecture for Family Development

A 10-year-old can meaningfully design a robot's personality using spec-driven workflows while parents handle the technical integration stack. This architecture combines **ESP32-S3-BOX** for voice capture, **Home Assistant** as the central hub, **Ollama** for personality-driven AI responses, and **CrowBot Bolt** for physical movement—all orchestrated through specification documents that Agatha can write and edit herself.

The key insight: the liatrio-labs spec-driven workflow uses plain Markdown that a child can edit, while the technical implementation translates those personality specifications into system prompts that Ollama uses for conversational AI. Voice flows through Home Assistant's pipeline, and robot commands dispatch via MQTT to the CrowBot.

---

## The spec-driven workflow creates a bridge between ideas and implementation

The **liatrio-labs/spec-driven-workflow** methodology provides exactly the structure needed for this project. It uses plain Markdown files as the single source of truth, with no dependencies or special tools required—just a text editor. The workflow follows four stages: **Spec → Tasks → Implementation → Validation**, with AI serving as a guided collaborator that asks structured questions and generates detailed artifacts.

For Agatha, the specification format can be simplified into a **"Robot Personality Worksheet"** with child-friendly prompts:

| Technical Concept | Kid-Friendly Version |
|-------------------|----------------------|
| User Stories | "Who is BOT for? What do they need?" |
| Functional Requirements | "When someone says _____, BOT should _____" |
| Non-Goals | "Things BOT should NEVER do" |
| Demoable Units | "Cool things I can show my friends" |

The most powerful adaptation is the **"If-Then Rules"** format—a conversation behavior table that Agatha writes in plain language:

```markdown
# BOT's Personality Specification
## Created by: Agatha

### If-Then Rules:
| When someone says... | BOT should... |
|---------------------|---------------|
| "Hello!" | Say "Hi there! I'm so happy to see you!" |
| "I'm bored" | Suggest a fun game or tell a joke |
| "I'm scared" | Speak softly: "I'm right here with you" |
| "Good night" | Say "Sweet dreams! See you tomorrow, friend!" |

### BOT's No-No List: 🚫
- Never be mean or sarcastic
- Never say scary things
- Never ignore someone who's sad
```

This format maps directly to Ollama system prompts—the technical parent simply converts Agatha's If-Then rules into the MODEL directive for a custom Ollama personality model.

---

## ESP32-S3-BOX provides a complete voice pipeline with minimal configuration

The **ESP32-S3-BOX-3** (or its predecessor) serves as the voice satellite in this architecture. It captures audio via dual microphones, detects wake words on-device, and streams to Home Assistant for processing.

**Wake word detection** runs directly on the ESP32-S3's chip using **microWakeWord**, executing inference in under **10ms**. Pre-trained models include "okay_nabu," "hey_jarvis," and "hey_mycroft"—or Agatha could request a custom wake word like "Hey BOT" (requires training via Google Colab with the microWakeWord framework).

The voice pipeline flows through these stages:
1. **Wake word triggers** (on-device, ~10ms latency)
2. **Audio streams to Home Assistant** via WiFi/ESPHome API
3. **Whisper converts speech to text** (local, 1-3 seconds)
4. **Ollama generates response** with BOT's personality (2-5 seconds)
5. **Piper synthesizes speech** with streaming (0.5-2 seconds)
6. **Audio returns to ESP32** speaker for playback

The complete ESPHome configuration for the voice satellite:

```yaml
voice_assistant:
  microphone: mic_id
  speaker: speaker_id
  use_wake_word: true
  noise_suppression_level: 2
  auto_gain: 31dBFS

micro_wake_word:
  models:
    - model: okay_nabu  # Or custom "hey_bot" model
```

**Expected round-trip latency**: 3-10 seconds for a fully local setup. Streaming TTS reduces perceived latency to ~2 seconds for the first sentence.

---

## Home Assistant orchestrates voice, AI, and robot control

Home Assistant serves as the central hub connecting all components. The **Voice Preview Edition (Voice PE)** offers superior audio quality with its dedicated XMOS chip for echo cancellation and noise removal, but the ESP32-S3-BOX works excellently as a lower-cost alternative with its touchscreen for visual feedback.

### Ollama integration for personality-driven conversation

The official Home Assistant Ollama integration (introduced in 2024.4) connects directly to a remote Ollama server. Configuration is straightforward:

- **URL**: `http://192.168.1.100:11434` (your Ollama server)
- **Model**: `llama3.2:3b` (best speed/quality balance)
- **Context Window**: 4096 tokens recommended for voice
- **Control Home Assistant**: Enable for device control via voice

**Agatha's personality specs translate directly into the system prompt**:

```dockerfile
# Ollama Modelfile for BOT
FROM llama3.2:3b

SYSTEM """You are BOT, a friendly household robot assistant.
Your personality: cheerful, helpful, with occasional silly humor.
Keep responses under 2 sentences—you're speaking aloud!

RULES FROM AGATHA'S SPECIFICATION:
- When greeting: Be excited and ask about their day
- When someone is sad: Offer comfort and suggest a game
- When asked to tell a joke: Pick from the approved joke list
- NEVER: Be mean, scary, or ignore emotions

Speak in a warm, friendly voice. You ARE a robot—make occasional 
beep-boop sounds when processing!"""

PARAMETER temperature 0.7
PARAMETER num_ctx 4096
PARAMETER num_predict 100  # Keep responses short
```

### Robot control through automations

Home Assistant's automation system bridges voice commands to CrowBot Bolt. When the Ollama conversation agent recognizes a movement intent, it triggers an automation that publishes MQTT commands:

```yaml
automation:
  - alias: "Voice Control Robot Forward"
    trigger:
      - platform: conversation
        command:
          - "move robot forward"
          - "go forward BOT"
          - "walk forward"
    action:
      - service: mqtt.publish
        data:
          topic: "crowbot/command"
          payload: "forward"
```

---

## CrowBot Bolt requires custom MQTT firmware for integration

The **CrowBot Bolt** is an ESP32-based educational robot from Elecrow with WiFi and Bluetooth built-in. It has no native Home Assistant integration, but its ESP32-WROVER-B brain can be reprogrammed with MQTT-enabled firmware.

**Critical GPIO pin mappings for motor control**:
- Motor Left−: GPIO12 / Motor Left+: GPIO13
- Motor Right−: GPIO14 / Motor Right+: GPIO15
- RGB LEDs: GPIO25 / Buzzer: GPIO33
- Ultrasonic sensor: GPIO27

**Arduino MQTT client code template** for CrowBot:

```cpp
#include <WiFi.h>
#include <PubSubClient.h>

PubSubClient mqtt(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String cmd = String((char*)payload).substring(0, length);
  
  if (cmd == "forward") Motor(0, 255, 0, 255);
  else if (cmd == "backward") Motor(255, 0, 255, 0);
  else if (cmd == "left") Motor(0, 100, 0, 255);
  else if (cmd == "right") Motor(0, 255, 0, 100);
  else if (cmd == "stop") Motor(0, 0, 0, 0);
  else if (cmd == "dance") danceSequence();
}

void Motor(int L1, int L2, int R1, int R2) {
  ledcWrite(12, L1); ledcWrite(13, L2);
  ledcWrite(14, R1); ledcWrite(15, R2);
}
```

**Voice-to-movement translation** follows this flow:

| Voice Command | Home Assistant Intent | MQTT Topic | CrowBot Action |
|---------------|----------------------|------------|----------------|
| "Move forward" | `robot_forward` | `crowbot/command: forward` | `Motor(0,255,0,255)` |
| "Turn left" | `robot_left` | `crowbot/command: left` | `Motor(0,100,0,255)` |
| "Do a dance!" | `robot_dance` | `crowbot/command: dance` | Custom sequence |

---

## Agatha's development workflow uses visual tools and templates

The spec-driven approach works beautifully for family development because **specifications are just Markdown files**—no programming required. Agatha writes her personality rules in a visual editor, commits them through GitHub, and the technical parent converts them to system prompts.

### Recommended tool progression

| Phase | Tool | Why |
|-------|------|-----|
| **Writing specs** | Typora or HackMD | WYSIWYG Markdown—feels like Word |
| **Version control** | GitHub Web Editor → Desktop | Start browser-only, graduate to app |
| **Prototyping responses** | Scratch | Visual blocks with speech synthesis |
| **Understanding flows** | Node-RED | Drag-and-drop logic connections |

**GitHub workflow for a 10-year-old**:
1. Parent creates a branch called `agathas-ideas`
2. Agatha edits `specs/bot-personality.md` via GitHub's web editor (pencil icon)
3. Commits with a message like "Added BOT's silly jokes"
4. Parent reviews and provides feedback as comments
5. Family merges together—celebration moment!

### Gamification makes development engaging

Create an **achievement system** to celebrate milestones:
- 🌟 **First Commit**: Made first change to BOT's personality
- 🎭 **Personality Designer**: Created 10 conversation responses
- 😂 **Comedy Writer**: Added 5 jokes to BOT's repertoire
- 💜 **Empathy Expert**: Designed responses for sad/scared/frustrated feelings
- 🎯 **Bug Hunter**: Found something BOT said wrong and fixed it

**Quest-based development** breaks work into fun challenges:
- Quest 1: Give BOT greetings for morning, afternoon, and evening
- Quest 2: Create 5 jokes BOT can tell when asked
- Quest 3: Design how BOT responds to emotions
- Boss Level: Write a full conversation about Agatha's favorite topic

---

## Complete system architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        AGATHA'S WORKFLOW                            │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐          │
│  │ Typora/HackMD │───▶│ GitHub Repo  │───▶│ System Prompt │          │
│  │ (Specs)       │    │ (Version Ctrl)│    │ (Modelfile)   │          │
│  └──────────────┘    └──────────────┘    └──────────────┘          │
└─────────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────────┐
│                      TECHNICAL IMPLEMENTATION                        │
│                                                                      │
│  ┌──────────────┐    ┌──────────────────┐    ┌──────────────┐      │
│  │ESP32-S3-BOX  │───▶│  HOME ASSISTANT   │───▶│ OLLAMA       │      │
│  │              │WiFi│                   │HTTP│ (llama3.2:3b)│      │
│  │ • Wake Word  │    │ • Voice Pipeline  │    │              │      │
│  │ • Audio I/O  │    │ • Whisper STT     │    │ • BOT's      │      │
│  │ • Display    │    │ • Piper TTS       │    │   Personality│      │
│  └──────────────┘    │ • Automations     │    └──────────────┘      │
│                      └────────┬──────────┘                          │
│                               │MQTT                                  │
│                               ▼                                      │
│                      ┌──────────────┐                                │
│                      │ CROWBOT BOLT │                                │
│                      │              │                                │
│                      │ • Motors     │                                │
│                      │ • LEDs       │                                │
│                      │ • Sensors    │                                │
│                      └──────────────┘                                │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Implementation roadmap for the family

### Week 1-2: Foundation
- [ ] Set up Home Assistant with Whisper, Piper, and Ollama integrations
- [ ] Flash ESP32-S3-BOX with ESPHome voice assistant firmware
- [ ] Create GitHub repository with spec-driven-workflow template
- [ ] Install Typora for Agatha's computer
- [ ] Agatha writes first "BOT Personality Sheet" using template

### Week 3-4: Voice pipeline
- [ ] Test wake word detection ("Hey BOT" or "Okay Nabu")
- [ ] Configure Ollama with initial system prompt from Agatha's specs
- [ ] Verify full voice loop: wake → speak → response → audio
- [ ] Agatha adds 10 more If-Then conversation rules

### Week 5-6: Robot integration
- [ ] Flash CrowBot Bolt with MQTT Arduino firmware
- [ ] Create Home Assistant automations for movement commands
- [ ] Test voice-to-movement: "BOT, move forward" → robot moves
- [ ] Agatha writes movement personality specs ("How should BOT dance?")

### Ongoing: Iteration
- Weekly family "Demo Day" to show BOT's new abilities
- Agatha owns the `specs/` folder; parent owns `firmware/`
- Celebrate each merged pull request
- Expand BOT's capabilities based on Agatha's imagination

---

## Conclusion

This architecture separates **creative ownership** (Agatha's personality specifications) from **technical implementation** (voice pipeline, AI integration, robot control). The spec-driven workflow provides structure without complexity—Agatha writes Markdown documents describing how BOT should behave, and those documents directly inform the Ollama system prompts that give BOT its personality.

The most important design decision: **making BOT's personality editable in plain text files**. When Agatha wants BOT to tell different jokes or respond more kindly to sadness, she edits a Markdown file, commits it to GitHub, and the change flows through to the AI. This gives her genuine ownership over BOT's character while building real skills in documentation, version control, and computational thinking.

The technical stack is deliberately local and privacy-focused—no cloud AI services required. Ollama runs on a home server, Whisper and Piper process speech locally, and all data stays within the home network. BOT becomes a true family creation: Agatha's imagination, powered by open-source AI, speaking through hardware the whole family assembled together.