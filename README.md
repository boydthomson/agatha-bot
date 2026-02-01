# 🤖 Agatha's BOT Project

An AI robot friend designed by Agatha (age 10) and built by the family!

## What is BOT?

BOT is a voice-controlled robot that can:
- **Talk** - Listen and respond using AI
- **Move** - Drive around using the CrowBot Bolt
- **Help** - Answer questions and tell jokes
- **Express** - Show emotions through lights and sounds

## 👨‍👩‍👧 How We Work Together

| Person | What They Do |
|--------|--------------|
| **Agatha** | Writes BOT's personality specs - decides *what* BOT says and does |
| **Parent** | Handles the technical stuff - code, wiring, configuration |

## 📁 Project Structure

```
agatha-bot/
├── specs/                    # 🌟 AGATHA'S FOLDER - Personality specs go here!
│   └── bot-personality.md    # How BOT behaves and responds
├── docs/
│   ├── specs/               # Detailed technical specifications
│   └── architecture/        # System design documents
├── prompts/                 # AI prompts for development workflow
└── firmware/                # Code for the hardware (parent's folder)
```

## 🚀 Quick Start for Agatha

### Writing Your First Spec

1. Open the file `specs/bot-personality.md`
2. Fill in the sections about how you want BOT to behave
3. Save it and tell your parent when you're ready to test!

### Example: Adding a New Response

In `specs/bot-personality.md`, add a row to the "If-Then Rules" table:

```markdown
| When someone says... | BOT should... |
|---------------------|---------------|
| "Tell me a joke"    | Say a funny joke and laugh |
```

## 🛠 Technical Stack (For Parents)

- **Voice Input**: ESP32-S3-BOX or Home Assistant Voice PE
- **AI Brain**: Ollama running locally (llama3.2:3b)
- **Smart Home**: Home Assistant for orchestration
- **Robot Body**: CrowBot Bolt (ESP32-based)
- **Communication**: MQTT for robot commands

## 📚 Development Workflow

We use the [spec-driven workflow](https://github.com/liatrio-labs/spec-driven-workflow) methodology:

1. **Spec** → Agatha writes what BOT should do
2. **Tasks** → We break it into small pieces
3. **Build** → Parent implements the technical parts
4. **Test** → Family tests BOT together
5. **Demo** → Show off what BOT can do!

## 🏆 Agatha's Achievements

Track your progress here!

- [ ] 🌟 **First Commit** - Made your first change to BOT
- [ ] 🎭 **Personality Designer** - Created 10 conversation responses
- [ ] 😂 **Comedy Writer** - Added 5 jokes
- [ ] 💜 **Empathy Expert** - Designed emotional responses
- [ ] 🎯 **Bug Hunter** - Found and fixed a BOT behavior
- [ ] 🎨 **Customizer** - Changed BOT's wake word or voice

## 📖 Documentation

- [Architecture Overview](docs/architecture/README.md)
- [Writing Specs Guide](docs/WRITING-SPECS.md)
- [Technical Setup](docs/SETUP.md)

---

*Made with ❤️ by the family*
