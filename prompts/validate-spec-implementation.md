# Validate Spec Implementation

## Purpose
This prompt helps verify that BOT's implementation matches the specifications. It's especially useful for checking that Agatha's personality specs are working correctly.

## Usage
Use this after implementing changes to verify everything works as specified.

---

## Prompt

```
SDD4️⃣ You are validating that BOT's implementation matches the specifications.

## Validation Process

### Step 1: Locate Spec
If not provided, find the most recent spec in `docs/specs/` that has incomplete validation.

### Step 2: Gather Evidence
- Check proof artifacts in `[spec]/[NN]-proofs/`
- Review task completion status
- Examine relevant code/config files

### Step 3: Test Against Spec

For **Personality Specs** (`specs/bot-personality.md`):
- [ ] Each If-Then rule has been implemented
- [ ] No-No rules are respected
- [ ] Wake word works correctly
- [ ] Light colors match spec (if applicable)
- [ ] Sound effects match spec (if applicable)

For **Technical Specs**:
- [ ] All functional requirements implemented
- [ ] Non-goals are not accidentally implemented
- [ ] Success metrics can be measured
- [ ] Integration points working

### Step 4: Create Validation Report

Save to: `docs/specs/[NN]-spec-[feature]/[NN]-validation-report.md`

## Validation Report Template

```markdown
# Validation Report: [Feature Name]

**Spec:** [path to spec]
**Validated:** [date]
**Status:** ✅ PASS / ⚠️ PARTIAL / ❌ FAIL

## Summary
[One paragraph overview]

## Coverage Matrix

### Personality Spec Coverage (If Applicable)

| If-Then Rule | Implemented | Tested | Notes |
|--------------|-------------|--------|-------|
| "Hello" greeting | ✅ | ✅ | Working great! |
| "I'm sad" response | ✅ | ⚠️ | Needs more testing |

### Technical Requirements Coverage

| Requirement | Status | Evidence |
|-------------|--------|----------|
| REQ-1: [desc] | ✅ | proof artifact link |
| REQ-2: [desc] | ❌ | [reason] |

## Test Results

### Voice Tests
| Test | Result |
|------|--------|
| Wake word detection | ✅ |
| Speech-to-text accuracy | ✅ |
| Response generation | ⚠️ |

### Robot Tests  
| Test | Result |
|------|--------|
| Forward movement | ✅ |
| Turn commands | ✅ |

## Issues Found

### Critical 🔴
[None / List issues]

### Minor 🟡
[None / List issues]

## Recommendations
[What to do next]

## Sign-Off

- [ ] Agatha tested and approves behavior
- [ ] Parent verified technical implementation
- [ ] Demo completed successfully
```

## Family Testing Tips

### Agatha's Testing Checklist 👧
- [ ] Say the wake word - does BOT listen?
- [ ] Try each greeting - does BOT respond correctly?
- [ ] Say "I'm sad" - is BOT kind and helpful?
- [ ] Ask for a joke - is it funny and appropriate?
- [ ] Tell BOT to move - does it work?
- [ ] Is BOT fun to talk to?

### Parent's Testing Checklist 🔧
- [ ] Voice pipeline latency acceptable?
- [ ] Ollama responses following system prompt?
- [ ] MQTT commands reaching CrowBot?
- [ ] Error handling working?
- [ ] Logs showing expected behavior?

## Validation Gates

The following must pass before marking complete:

**GATE A:** All critical requirements implemented
**GATE B:** No blocking bugs found
**GATE C:** Agatha has tested and approved
**GATE D:** Demo completed successfully
```

---

## Example Validation Session

**User:** Let's validate the basic personality spec

**AI:** SDD4️⃣ I'll validate `specs/bot-personality.md` against the current implementation.

**Checking If-Then Rules:**

| Rule | Status | Notes |
|------|--------|-------|
| "Hello!" → excited greeting | ✅ | Tested, working |
| "Good morning" → morning greeting | ✅ | Working |
| "I'm sad" → comfort response | ⚠️ | Response is good but could be warmer |
| "Tell me a joke" → tells joke | ✅ | Using approved jokes list |

**Checking No-No Rules:**
- ✅ Not mean or sarcastic
- ✅ No scary content
- ✅ Responds to sadness (doesn't ignore)

**Recommendation:** 
Task 2 (Emotional Responses) needs a small tweak. The "I'm sad" response could start with "I'm sorry you're feeling sad" to feel warmer.

Would you like me to create the full validation report?
