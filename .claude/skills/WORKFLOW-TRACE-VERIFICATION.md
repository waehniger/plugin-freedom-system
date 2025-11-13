# Workflow Trace Verification

**Post-Fix Status**: ✅ All decision menu routing issues resolved

This document traces the complete workflow progression to verify seamless skill invocation.

---

## Happy Path: Ideation → UI Mockup → Planning → Implementation

### Path 1: Traditional Workflow (Full UI Mockup First)

**User action**: `/dream "MyPlugin"`

**Step 1: plugin-ideation**
- Creates `creative-brief.md`
- Shows decision menu (line 487)
- User selects: **"2. Full UI mockup first"**

**Handler execution** (line 361-365):
```xml
<delegation_rule skill="ui-mockup" trigger="option_2">
- Action: Must invoke ui-mockup skill via Skill tool
</delegation_rule>
```
✅ **Invokes**: `ui-mockup` skill

---

**Step 2: ui-mockup**
- Creates `v1-ui.yaml` and `v1-ui-test.html`
- Shows Phase 5.5 decision menu (line 620)
- User selects: **"2. Finalize"**

**Handler execution** (line 658):
- Proceeds to Phase 5.6 (automatic brief update)
- Generates implementation files (Phase 6-10)
- Shows Phase 10.7 completion menu (decision-menus.md line 99)
- User selects: **"1. Start implementation"**

**Handler execution** (decision-menus.md line 110-115):
```markdown
Invoke Skill tool:
- skill: "plugin-workflow"
- prompt: "Continue workflow for [PluginName] from Stage 1"
```
✅ **Invokes**: `plugin-workflow` skill

---

**Step 3: plugin-workflow**
- Detects missing `architecture.md` and `plan.md`
- Routes to `plugin-planning` skill for Stage 0

---

**Step 4: plugin-planning**
- Invokes `research-planning-agent` subagent
- Creates `architecture.md` and `plan.md`
- Shows decision menu (line 141)
- User selects: **"1. Start implementation"**

**Handler execution** (line 161-168):
```markdown
Invoke plugin-workflow skill via Skill tool:
  ```
  Skill tool:
  - skill: "plugin-workflow"
  - prompt: "Begin implementation for [PluginName] starting at Stage 2"
  ```
```
✅ **Invokes**: `plugin-workflow` skill

---

**Step 5: plugin-workflow**
- Proceeds to Stage 2 (Foundation)
- Invokes `foundation-shell-agent` subagent
- Continues through Stages 3-5
- Plugin complete

**Result**: Zero manual command invocations required. Seamless progression.

---

## Path 2: Quick Params + Parallel Workflow (18 min faster)

**User action**: `/dream "MyPlugin"`

**Step 1: plugin-ideation**
- Creates `creative-brief.md`
- Shows decision menu (line 487)
- User selects: **"1. Quick params + parallel workflow"**

**Handler execution** (line 355-359):
- Executes Phase 8.1 (Quick Parameter Capture)
- Creates `parameter-spec-draft.md`
- Shows parallel workflow menu (line 502)

---

**Step 2: plugin-ideation (parallel menu)**
- User selects: **"1. Start Stage 0 research now"**

**Handler execution** (line 514-519):
```markdown
Invoke plugin-planning skill for Stage 0 (will accept draft params)
  ```
  Skill tool:
  - skill: "plugin-planning"
  - prompt: "Begin Stage 0 research for [PluginName] using draft parameter-spec"
  ```
```
✅ **Invokes**: `plugin-planning` skill

---

**Step 3: plugin-planning**
- Accepts draft `parameter-spec-draft.md`
- Creates `architecture.md` and `plan.md`
- Shows decision menu (line 141)
- User selects: **"1. Start implementation"**

**Handler execution** (line 161-168):
✅ **Invokes**: `plugin-workflow` skill

---

**Step 4: plugin-workflow**
- Proceeds to Stage 2 (Foundation)
- Plugin implementation continues

**Result**: Zero manual command invocations required. Seamless progression.

---

## Path 3: Parallel Execution (Maximum Time Savings)

**User action**: `/dream "MyPlugin"`

**Step 1: plugin-ideation**
- Creates `creative-brief.md`
- Shows decision menu (line 487)
- User selects: **"1. Quick params + parallel workflow"**

**Handler execution**:
- Executes Phase 8.1 (Quick Parameter Capture)
- Creates `parameter-spec-draft.md`
- Shows parallel workflow menu (line 502)

---

**Step 2: plugin-ideation (parallel menu)**
- User selects: **"3. Do both in parallel"**

**Handler execution** (line 528-534):
```markdown
Invoke plugin-planning skill first, then suggest ui-mockup in parallel
  ```
  Skill tool:
  - skill: "plugin-planning"
  - prompt: "Begin Stage 0 research for [PluginName]. After completion, suggest running ui-mockup in parallel."
  ```
After plugin-planning completes, present option to start ui-mockup.
```
✅ **Invokes**: `plugin-planning` skill
✅ **Suggests**: `ui-mockup` skill after completion

---

**Step 3: plugin-planning**
- Completes Stage 0 research
- Shows decision menu with suggestion to run ui-mockup
- User can proceed to implementation OR start ui-mockup

**Result**: Maximum flexibility. User can run DSP research and UI design in parallel sessions. Zero manual command invocations.

---

## Verification Summary

### Decision Menus Audited: 7

1. ✅ **plugin-ideation** - After creative brief (line 487)
2. ✅ **plugin-ideation** - After quick params (line 502)
3. ✅ **plugin-ideation** - Vagueness detection (line 568)
4. ✅ **plugin-ideation** - After improvement brief (line 587)
5. ✅ **plugin-planning** - After Stage 0 complete (line 141)
6. ✅ **ui-mockup** - Phase 5.5 design approval (line 620)
7. ✅ **ui-mockup** - Phase 10.7 completion (decision-menus.md line 99)

### Issues Found: 2

1. ❌ **plugin-planning** (line 141) - Missing delegation handler → ✅ **FIXED** (lines 152-218)
2. ❌ **plugin-ideation** (line 516) - Manual command instruction → ✅ **FIXED** (lines 528-534)

### Issues Remaining: 0

All decision menus now invoke skills directly via Skill tool. Zero manual command execution required for workflow progression.

---

## Pattern Consistency

All handlers follow the delegation_rule pattern:

```xml
<delegation_rule skill="SKILL_NAME" trigger="option_N">
- Condition: User chooses option N
- Action: Must invoke SKILL_NAME skill via Skill tool
</delegation_rule>
```

Followed by explicit handler code:

```markdown
- **Option N: Action name**

  Invoke SKILL_NAME skill via Skill tool:
  ```
  Skill tool:
  - skill: "SKILL_NAME"
  - prompt: "Context-specific prompt for this invocation"
  ```
```

This pattern appears in:
- `plugin-ideation/SKILL.md` (lines 355-377, 514-536)
- `plugin-planning/SKILL.md` (lines 155-218)
- `ui-mockup/references/decision-menus.md` (lines 110-115)

**Consistency**: ✅ **VERIFIED**

---

## Files Modified

1. `.claude/skills/plugin-planning/SKILL.md` - Added delegation handler (lines 152-218)
2. `.claude/skills/plugin-ideation/SKILL.md` - Fixed option 3 handler (lines 514-536)
3. `.claude/skills/plugin-planning/assets/decision-menu-stage-0.md` - Added handler note
4. `.claude/skills/plugin-planning/assets/decision-menu-stage-1.md` - Updated option 1 text

**Total changes**: 4 files
**Lines added**: ~70 lines (delegation handlers + documentation)
**Lines removed**: ~1 line (manual command instruction)

---

## Test Recommendation

To verify fixes work in practice:

1. Run `/dream "TestPlugin"`
2. Complete creative brief
3. Select "2. Full UI mockup first"
4. Verify ui-mockup skill invoked automatically (no manual command)
5. Complete mockup, select "2. Finalize"
6. Complete implementation files, select "1. Start implementation"
7. Verify plugin-planning or plugin-workflow invoked automatically
8. Complete Stage 0 planning, select "1. Start implementation"
9. Verify plugin-workflow Stage 2 invoked automatically (no manual command)

**Expected result**: Zero manual `/implement`, `/plan`, or `/dream` commands required.

---

## Impact

**Before**: User must manually run `/implement`, `/plan`, or `/dream` at 2-3 points in workflow
**After**: All workflow transitions happen automatically via skill invocations

**Time saved**: ~20-30 seconds per plugin (eliminated manual command typing)
**UX improvement**: Workflow feels automated vs manual checklist
**Friction reduced**: From "run this command" to "automatically continues"

This is the difference between a **guided checklist** and an **automated workflow**.
