# Generate Task List from Spec

## Purpose
This prompt helps break down specifications into actionable tasks. It creates a task list that both Agatha and the parent can work from.

## Usage
Point your AI assistant to a spec file, then use this prompt to generate tasks.

---

## Prompt

```
SDD2️⃣ You are helping create a task list for the BOT project from a specification.

## Your Process

### Phase 1: Analysis (Internal)
1. Read the spec carefully
2. Identify all requirements
3. Group related items
4. Determine what's needed from Agatha vs Parent

### Phase 2: Parent Tasks
Generate high-level tasks with:
- Clear description
- Who does it (Agatha 👧 or Parent 🔧)
- What can be demoed when complete
- Estimated difficulty (🟢 Easy, 🟡 Medium, 🔴 Hard)

Present these for review before continuing.

### Phase 3: Sub-Tasks (after confirmation)
Break each parent task into specific sub-tasks with:
- [ ] Checkbox format
- Relevant files to create/modify
- Clear completion criteria

## Output Format

Save to: `docs/specs/[NN]-spec-[feature]/[NN]-tasks-[feature].md`

```markdown
# Tasks: [Feature Name]

**Spec:** [link to spec]
**Created:** [date]

## Overview
[Brief description of what we're building]

## Task List

### Task 1: [Name] 👧/🔧 🟢/🟡/🔴
**Demo:** [What can we show when this is done?]
**Owner:** Agatha / Parent

Sub-tasks:
- [ ] Sub-task 1
- [ ] Sub-task 2

**Relevant Files:**
- `path/to/file.md`

---
[continue for each task]
```

## Task Assignment Guidelines

**Agatha's Tasks 👧:**
- Writing If-Then rules
- Choosing responses and behaviors  
- Picking jokes, stories, content
- Testing and giving feedback
- Designing light colors and sounds

**Parent's Tasks 🔧:**
- Writing code and configuration
- Hardware setup and wiring
- Debugging technical issues
- Converting specs to system prompts
- Integration and deployment

## Remember
- Keep Agatha's tasks fun and achievable
- Every task should have something demoable
- Technical tasks should reference Agatha's specs
```

---

## Example Output

```markdown
# Tasks: Bedtime Stories Feature

**Spec:** docs/specs/02-spec-bedtime-stories/02-spec-bedtime-stories.md
**Created:** 2024-01-15

## Overview
Adding the ability for BOT to tell bedtime stories when asked.

## Task List

### Task 1: Story Collection 👧 🟢
**Demo:** Show the list of 5 stories BOT can tell
**Owner:** Agatha

Sub-tasks:
- [ ] Pick 5 favorite short stories
- [ ] Write how BOT should introduce each story
- [ ] Add stories to approved content list

**Relevant Files:**
- `specs/bot-personality.md` (add Bedtime Stories section)
- `specs/story-content/` (new folder for stories)

---

### Task 2: Story-Telling Voice Commands 👧 🟢  
**Demo:** Say "tell me a story" and BOT responds correctly
**Owner:** Agatha

Sub-tasks:
- [ ] Write If-Then rules for story requests
- [ ] Define how BOT asks which story to tell
- [ ] Write BOT's "story time" introduction

**Relevant Files:**
- `specs/bot-personality.md` (If-Then Rules table)

---

### Task 3: Ollama Story Prompt 🔧 🟡
**Demo:** BOT tells a complete story out loud
**Owner:** Parent

Sub-tasks:
- [ ] Create story-telling system prompt
- [ ] Add story content to Ollama context
- [ ] Configure response length for stories
- [ ] Test TTS with longer content

**Relevant Files:**
- `firmware/ollama/modelfiles/bot-storyteller.txt`
- `docs/specs/02-spec-bedtime-stories/02-proofs/`
```
