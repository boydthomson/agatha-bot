# Tasks: Basic Personality

**Spec:** [01-spec-basic-personality.md](01-spec-basic-personality.md)
**Created:** 2025-01-31
**Status:** Not Started

---

## Overview

Implement BOT's foundational personality based on Agatha's specification. This includes configuring Ollama with the right system prompt, testing the voice pipeline, and verifying all behaviors match the spec.

---

## Task List

### Task 1: Write Initial Personality Rules 👧 🟢
**Demo:** Show the completed personality spec with at least 10 If-Then rules
**Owner:** Agatha

Sub-tasks:
- [ ] Fill in the "Who is BOT for?" section
- [ ] Check personality trait boxes
- [ ] Write 3 greeting responses
- [ ] Write 4 emotional responses (sad, scared, bored, happy)
- [ ] Write 3 "Questions" responses
- [ ] Add 5 approved jokes
- [ ] Complete the No-No list

**Relevant Files:**
- `specs/bot-personality.md`

---

### Task 2: Create Ollama Modelfile 🔧 🟡
**Demo:** Run `ollama run bot` and have a basic conversation
**Owner:** Parent

Sub-tasks:
- [ ] Create Modelfile based on Agatha's personality spec
- [ ] Include all If-Then rules as behavioral guidelines
- [ ] Add No-No rules as system constraints
- [ ] Set appropriate parameters (temperature, max tokens)
- [ ] Build and test the model locally

**Relevant Files:**
- `firmware/ollama/Modelfile`
- `specs/bot-personality.md` (reference)

---

### Task 3: Configure Home Assistant Ollama Integration 🔧 🟡
**Demo:** Send a test message through Home Assistant and get a BOT-personality response
**Owner:** Parent

Sub-tasks:
- [ ] Add Ollama integration to Home Assistant
- [ ] Configure URL and model name
- [ ] Set up as conversation agent
- [ ] Test via Home Assistant Developer Tools
- [ ] Verify response follows personality spec

**Relevant Files:**
- Home Assistant configuration
- `docs/SETUP.md` (reference)

---

### Task 4: Test Voice Pipeline End-to-End 🔧 🟡
**Demo:** Say "Okay Nabu, hello!" and hear BOT respond in character
**Owner:** Parent (with Agatha for testing)

Sub-tasks:
- [ ] Verify ESP32-S3-BOX wake word detection
- [ ] Test Whisper speech-to-text accuracy
- [ ] Confirm Ollama receives and processes query
- [ ] Verify Piper TTS output sounds good
- [ ] Test full round-trip latency

**Relevant Files:**
- `firmware/esphome/bot-voice-satellite.yaml`
- Home Assistant voice pipeline config

---

### Task 5: Family Testing Session 👧🔧 🟢
**Demo:** Agatha tests all If-Then rules and confirms BOT behaves correctly
**Owner:** Family

Sub-tasks:
- [ ] Test all greeting variations
- [ ] Test all emotional responses
- [ ] Test joke telling
- [ ] Verify No-No rules are respected
- [ ] Note any responses that don't feel right
- [ ] Update personality spec based on testing

**Relevant Files:**
- `specs/bot-personality.md`
- `docs/specs/01-spec-basic-personality/01-proofs/01-task-05-proofs.md` (create)

---

## Relevant Files Summary

| File | Purpose | Owner |
|------|---------|-------|
| `specs/bot-personality.md` | BOT's personality rules | Agatha |
| `firmware/ollama/Modelfile` | Ollama configuration | Parent |
| `firmware/esphome/bot-voice-satellite.yaml` | ESP32 config | Parent |
| `docs/specs/01-spec-basic-personality/01-proofs/` | Test evidence | Both |

---

## Progress Tracking

| Task | Status | Completed |
|------|--------|-----------|
| Task 1: Personality Rules | [ ] Not Started | |
| Task 2: Ollama Modelfile | [ ] Not Started | |
| Task 3: HA Integration | [ ] Not Started | |
| Task 4: Voice Pipeline | [ ] Not Started | |
| Task 5: Family Testing | [ ] Not Started | |

**Legend:** `[ ]` Not started | `[~]` In progress | `[x]` Complete
