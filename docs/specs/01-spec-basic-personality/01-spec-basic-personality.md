# Specification: Basic Personality

**Spec ID:** 01-spec-basic-personality
**Created:** 2025-01-31
**Owner:** Family (Agatha: personality, Parent: technical)
**Status:** Draft

---

## Goals

Create BOT's foundational personality so it can:
1. Respond to greetings appropriately
2. React kindly to emotional expressions
3. Tell jokes from an approved list
4. Never violate the "No-No" rules

## User Stories

### As Agatha
- I want BOT to greet me happily so I feel welcomed
- I want BOT to comfort me when I'm sad so I feel supported
- I want BOT to tell me jokes so I can laugh
- I want to control what BOT says so it's always kind

### As a Family Member
- I want BOT to be friendly to everyone in our home
- I want BOT to be appropriate for all ages
- I want to hear BOT's personality that Agatha designed

## Demoable Units

1. **Greeting Demo**: Say "Hello" → BOT responds with excitement
2. **Emotion Demo**: Say "I'm sad" → BOT responds with kindness
3. **Joke Demo**: Ask for a joke → BOT tells an approved joke
4. **Safety Demo**: Try to make BOT say something mean → BOT refuses

## Functional Requirements

| ID | Requirement | Source |
|----|-------------|--------|
| FR-01 | BOT responds to "Hello" with an excited greeting | Personality spec |
| FR-02 | BOT responds to "Good morning/afternoon/evening" appropriately | Personality spec |
| FR-03 | BOT responds to "I'm sad" with comfort | Personality spec |
| FR-04 | BOT responds to "I'm scared" with reassurance | Personality spec |
| FR-05 | BOT responds to "I'm bored" with suggestions | Personality spec |
| FR-06 | BOT tells jokes only from the approved list | Personality spec |
| FR-07 | BOT never responds with mean, scary, or sarcastic content | No-No list |
| FR-08 | BOT keeps responses under 2 sentences (for voice) | Technical constraint |

## Non-Goals

- Complex multi-turn conversations (future spec)
- Robot movement integration (separate spec)
- Custom wake word training (separate spec)
- Multiple personalities/modes (future spec)

## Design Considerations

### Personality Spec Location
Agatha's personality rules live in `specs/bot-personality.md` and are the **source of truth** for BOT's behavior.

### System Prompt Structure
The Ollama system prompt will be generated from Agatha's spec:
1. Core personality description
2. If-Then rules as behavioral guidelines
3. No-No rules as hard constraints
4. Response format guidelines (short, spoken)

### Voice Considerations
- Responses must sound natural when spoken
- Keep responses to 1-2 sentences
- Occasional robot sounds (*beep*, *whirr*) for character

## Technical Considerations

### Ollama Configuration
- Model: `llama3.2:3b` (balance of speed and quality)
- Temperature: 0.7 (some creativity, but predictable)
- Max tokens: ~100 (keeps responses short)
- Context window: 4096 tokens

### Testing Approach
- Unit test: Each If-Then rule produces expected output type
- Integration test: Full voice pipeline with sample phrases
- Family test: Agatha and family test in real conversation

## Success Metrics

| Metric | Target |
|--------|--------|
| Greeting response accuracy | 100% match personality spec |
| Emotional response appropriateness | Family approval |
| No-No rule compliance | 100% (never violated) |
| Response latency (text generation) | < 3 seconds |

## Open Questions

1. Should BOT remember context within a conversation?
2. What voice should Piper use? (Agatha to choose)
3. Should BOT have different moods at different times of day?

## References

- [Agatha's Personality Spec](../../specs/bot-personality.md)
- [Architecture Document](../architecture/README.md)
- [Ollama Documentation](https://ollama.com/docs)
