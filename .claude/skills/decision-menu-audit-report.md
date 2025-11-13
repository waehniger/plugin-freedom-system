# Decision Menu Routing Audit Report

**Date**: 2025-11-13
**Scope**: All workflow skills (plugin-ideation, plugin-planning, ui-mockup, plugin-workflow, context-resume)
**Objective**: Ensure decision menus invoke skills directly instead of telling users to run slash commands

## Executive Summary

**Total decision menus found**: 7
**Issues identified**: 2
**Already correct**: 5

The workflow progression from ideation → ui-mockup → planning → implementation has two critical routing issues that break seamless workflow progression.

---

## Audit Findings by Skill

### 1. plugin-ideation/SKILL.md

**Decision Menus Found**: 4

#### Menu 1: After creative brief complete (line 487)
- **Location**: Line 487-498
- **Status**: ✅ **CORRECT** - Has delegation handler (lines 353-381)
- **Options**:
  1. Quick params + parallel workflow → Phase 8.1 (Quick Parameter Capture)
  2. Full UI mockup first → Invokes ui-mockup skill via Skill tool ✓
  3. Start implementation directly → Invokes plugin-workflow skill via Skill tool ✓
  4. Research similar plugins → Invokes deep-research skill via Skill tool ✓
  5. Save for later → Confirms handoff, exits
  6. Other → Collects custom input

#### Menu 2: After quick parameter capture (line 502)
- **Location**: Line 502-510
- **Status**: ⚠️ **ISSUE FOUND** - Option 3 handler incorrect
- **Problem**: Line 516 tells user to run `/plan` command instead of invoking skill
- **Current handler**: `Display: "Start Stage 0 research first (/plan [PluginName]), then run UI mockup..."`
- **Should be**: Invoke plugin-planning skill for option 1, ui-mockup skill for option 2, and BOTH for option 3
- **Fix required**: Yes

#### Menu 3: Vagueness detection in improvement mode (line 568)
- **Location**: Line 568-575
- **Status**: ✅ **CORRECT** - Handler present (lines 577-578)
- **Options**:
  1. Brainstorm approaches first → Continue with improvement brainstorming
  2. Implement something reasonable → Exit to plugin-improve skill

#### Menu 4: After improvement brief complete (line 587)
- **Location**: Line 587-597
- **Status**: ✅ **CORRECT** - Has delegation handler (lines 599-604)
- **Options**:
  1. Start implementation → Invokes plugin-improve skill ✓
  2. Research implementation approaches → Invokes deep-research skill ✓
  3. Review existing code first → Reads relevant files
  4. Save for later → Confirms handoff, exits
  5. Other → Collects custom input

### 2. plugin-planning/SKILL.md

**Decision Menus Found**: 1

#### Menu 1: After Stage 0 complete (line 141)
- **Location**: Line 141-150
- **Status**: ❌ **CRITICAL ISSUE** - NO delegation handler
- **Problem**: Menu is shown but NO handler section exists after it
- **Current state**: Menu shown, then jumps to "VALIDATION GATE" section (line 154)
- **Options**:
  1. Start implementation - Begin Stage 2 (recommended)
  2. Review contracts
  3. Improve creative brief
  4. Run deeper JUCE investigation
  5. Pause here
  6. Other
- **Fix required**: Yes - Add complete delegation handler after menu

**Additional context**:
- Line 199 shows handoff protocol telling user to run `/implement [PluginName]`
- This is AFTER the skill completes, not within the decision menu handler
- The decision menu at line 141 needs its own handler to invoke plugin-workflow directly

### 3. ui-mockup/SKILL.md

**Decision Menus Found**: 2

#### Menu 1: After Phase A complete (Phase 5.5, line 620)
- **Location**: Line 620-638
- **Status**: ✅ **CORRECT** - Has handler (lines 640-670)
- **Options**:
  1. Iterate → Collect feedback, return to Phase 2 with v[N+1]
  2. Finalize → Proceed to Phase 5.6, then Phase 6-10 ✓
  3. Save as template → Invokes ui-template-library skill ✓
  4. Other → Handles custom requests

#### Menu 2: After Phase B complete (Phase 10.7, referenced line 945-949)
- **Location**: Referenced in decision-menus.md (lines 89-128)
- **Status**: ✅ **CORRECT** - Handler documented in decision-menus.md
- **Options**:
  1. Start implementation → Invokes plugin-workflow skill ✓
  2. Create another UI version → Return to Phase 1 with v[N+1]
  3. Test in browser → Open HTML file
  4. Other → Handle custom requests

### 4. plugin-workflow/SKILL.md

**Decision Menus Found**: 0 (checkpoint menus only)

- **Status**: ✅ **CORRECT**
- **Note**: This skill presents checkpoint menus dynamically (line 715-730)
- **Pattern**: Generic template, not hardcoded, so routing is handled by orchestrator logic
- **No issues found**

### 5. context-resume/SKILL.md

**Decision Menus Found**: 0

- **Status**: ✅ **CORRECT**
- **Note**: This skill routes to other skills but doesn't present decision menus itself
- **No issues found**

---

## Issues Requiring Fixes

### Issue 1: plugin-planning/SKILL.md - Missing delegation handler (CRITICAL)

**Severity**: CRITICAL - Breaks workflow progression from planning to implementation

**Location**: After line 150 (after the decision menu)

**Problem**: User completes Stage 0 planning, selects "1. Start implementation", but nothing happens because there's no handler code.

**Fix**: Add delegation handler section with proper skill invocations

**Impact**: This is the primary bottleneck in the workflow. Users must manually run `/implement` command instead of seamless progression.

---

### Issue 2: plugin-ideation/SKILL.md - Incorrect handler for option 3 (MODERATE)

**Severity**: MODERATE - Parallel workflow path broken

**Location**: Line 516

**Problem**: After quick parameter capture, option 3 ("Do both in parallel") tells user to run `/plan` command instead of invoking plugin-planning skill directly.

**Current code**:
```markdown
- **Option 3:** Display: "Start Stage 0 research first (/plan [PluginName]), then run UI mockup (/dream [PluginName] → option 3) in a separate session or after Stage 0 completes."
```

**Fix**: Change to invoke plugin-planning skill directly, suggest ui-mockup in parallel

**Impact**: Parallel workflow path (18 min faster) is broken, forcing users to manual command execution.

---

## Correct Patterns Found (Reference Examples)

### Example 1: plugin-ideation delegation_rule pattern (lines 352-381)

```xml
<delegation_rule skill="ui-mockup" trigger="option_2">
- Condition: User chooses option 2 (Full UI mockup first)
- Action: Must invoke ui-mockup skill via Skill tool
- Do NOT attempt to create mockup within this skill
</delegation_rule>
```

**Key pattern**: Uses XML-style delegation_rule tags with clear skill invocation instructions

### Example 2: ui-mockup decision-menus.md pattern (lines 109-115)

```markdown
**Option 1: Start implementation**
- Invoke plugin-workflow skill:
  ```
  Invoke Skill tool:
  - skill: "plugin-workflow"
  - prompt: "Continue workflow for [PluginName] from Stage 1"
  ```
```

**Key pattern**: Clear "Invoke Skill tool" instruction with skill name and prompt

---

## Recommended Fixes

### Fix 1: plugin-planning/SKILL.md

**Insert after line 150** (after the decision menu, before VALIDATION GATE section):

```markdown
<delegation_rules>
**Handle menu choices:**

<delegation_rule skill="plugin-workflow" trigger="option_1">
- Condition: User chooses option 1 (Start implementation)
- Action: Must invoke plugin-workflow skill via Skill tool
- Prompt: "Begin implementation for [PluginName] starting at Stage 2"
</delegation_rule>

- **Option 1: Start implementation**

  Invoke plugin-workflow skill via Skill tool:
  ```
  Skill tool:
  - skill: "plugin-workflow"
  - prompt: "Begin implementation for [PluginName] starting at Stage 2"
  ```

- **Option 2: Review contracts**

  Read and display files:
  - `plugins/[PluginName]/.ideas/architecture.md`
  - `plugins/[PluginName]/.ideas/plan.md`
  - `plugins/[PluginName]/.ideas/parameter-spec.md`
  - `plugins/[PluginName]/.ideas/creative-brief.md`

  After displaying, return to decision menu.

- **Option 3: Improve creative brief**

  Invoke plugin-ideation skill in improvement mode:
  ```
  Skill tool:
  - skill: "plugin-ideation"
  - prompt: "Improve creative brief for [PluginName] based on Stage 0 research findings"
  ```

<delegation_rule skill="deep-research" trigger="option_4">
- Condition: User chooses option 4 (Run deeper investigation)
- Action: Must invoke deep-research skill via Skill tool
</delegation_rule>

- **Option 4: Run deeper JUCE investigation**

  Invoke deep-research skill via Skill tool:
  ```
  Skill tool:
  - skill: "deep-research"
  - prompt: "Investigate JUCE implementation patterns for [PluginName] - focus on [topic from architecture.md]"
  ```

- **Option 5: Pause here**

  Confirm state saved:
  - Verify `.continue-here.md` shows stage: 0, status: complete
  - Verify `PLUGINS.md` shows 🚧 Stage 0
  - Display: "Stage 0 complete. Resume anytime with /continue [PluginName]"
  - Exit skill

- **Option 6: Other**

  Collect custom input and handle appropriately:
  - If request relates to implementation: Route to plugin-workflow
  - If request relates to design: Route to ui-mockup
  - If request relates to research: Route to deep-research
  - Otherwise: Handle directly or exit
</delegation_rules>
```

---

### Fix 2: plugin-ideation/SKILL.md

**Replace line 516** with:

```markdown
- **Option 1:** Invoke plugin-planning skill for Stage 0 (will accept draft params)
  ```
  Skill tool:
  - skill: "plugin-planning"
  - prompt: "Begin Stage 0 research for [PluginName] using draft parameter-spec"
  ```

- **Option 2:** Invoke ui-mockup skill (will validate against draft when generating full spec)
  ```
  Skill tool:
  - skill: "ui-mockup"
  - prompt: "Create UI mockup for [PluginName] using parameter-spec-draft.md"
  ```

- **Option 3:** Invoke plugin-planning skill first, then suggest ui-mockup in parallel
  ```
  Skill tool:
  - skill: "plugin-planning"
  - prompt: "Begin Stage 0 research for [PluginName]. After completion, suggest running ui-mockup in parallel."
  ```
  After plugin-planning completes, present option to start ui-mockup.

- **Option 4:** Collect custom input
```

---

## Asset File Updates (Non-Blocking)

These are template files for reference, not executable code:

### Update 1: plugin-planning/assets/decision-menu-stage-0.md

**Line 8-15**: Already shows the menu correctly, but should include handler documentation for consistency.

**Action**: Add note at bottom of file referencing the delegation pattern in SKILL.md

### Update 2: plugin-planning/assets/decision-menu-stage-1.md

**Line 4-11**: Already shows the menu correctly, references `/implement` command.

**Action**: Update to reference plugin-workflow skill invocation instead of `/implement` command.

---

## Verification Checklist

After applying fixes:

- [x] **plugin-planning/SKILL.md** has delegation handler after line 150
- [x] **plugin-ideation/SKILL.md** line 516 invokes skills instead of showing commands
- [x] All "Start implementation" options invoke plugin-workflow skill
- [x] All "Create mockup" options invoke ui-mockup skill
- [x] All "Research" options invoke plugin-planning or deep-research skills
- [x] No handlers tell users to run `/command` for workflow progression
- [x] Pattern matches existing working examples (plugin-ideation lines 353-381, ui-mockup decision-menus.md)
- [x] Asset files updated for consistency (decision-menu-stage-0.md, decision-menu-stage-1.md)

---

## Happy Path Test Trace

**Expected workflow**: ideation → ui-mockup → planning → implementation

1. **Start**: User runs `/dream "MyPlugin"`
2. **plugin-ideation**: Creates creative-brief.md → Shows menu → User selects "2. Full UI mockup first"
   - ✅ **CORRECT**: Invokes ui-mockup skill (line 361-365)
3. **ui-mockup**: Creates mockup files → Shows Phase 5.5 menu → User selects "2. Finalize"
   - ✅ **CORRECT**: Proceeds to Phase 6-10, generates implementation files
   - Shows Phase 10.7 menu → User selects "1. Start implementation"
   - ✅ **CORRECT**: Invokes plugin-workflow skill (decision-menus.md line 110-115)
4. **plugin-workflow**: Detects missing planning contracts → Routes to plugin-planning
5. **plugin-planning**: Completes Stage 0 → Shows menu (line 141) → User selects "1. Start implementation"
   - ❌ **BROKEN**: NO HANDLER - workflow stops here
   - **Expected**: Should invoke plugin-workflow skill
   - **Current**: User must manually run `/implement MyPlugin`

**Alternative path**: ideation → quick params → planning

1. **Start**: User runs `/dream "MyPlugin"`
2. **plugin-ideation**: Creates creative-brief.md → Shows menu → User selects "1. Quick params + parallel workflow"
   - ✅ **CORRECT**: Executes Phase 8.1 (Quick Parameter Capture)
3. **plugin-ideation**: Creates parameter-spec-draft.md → Shows menu (line 502) → User selects "1. Start Stage 0 research now"
   - ✅ **CORRECT**: Invokes plugin-planning skill (line 514)
4. **plugin-planning**: Completes Stage 0 → Shows menu (line 141) → User selects "1. Start implementation"
   - ❌ **BROKEN**: NO HANDLER - workflow stops here

**Alternative path with parallel option**:

1. **plugin-ideation**: Quick params complete → User selects "3. Do both in parallel"
   - ❌ **BROKEN**: Line 516 tells user to run `/plan` command instead of invoking plugin-planning
   - **Expected**: Should invoke plugin-planning skill, then suggest ui-mockup
   - **Current**: User must manually run `/plan MyPlugin`

---

## Impact Analysis

### Pre-Fix User Experience
1. Complete creative brief
2. Select "Start implementation"
3. See message: "Ready to build. Run: /implement [PluginName]"
4. User manually types `/implement MyPlugin`
5. Workflow continues

**Friction points**: 2 manual command invocations in critical path

### Post-Fix User Experience
1. Complete creative brief
2. Select "Start implementation"
3. **Automatically proceeds to plugin-workflow**
4. Workflow continues seamlessly

**Friction points**: 0 manual command invocations

**Time savings**: ~10 seconds per workflow transition × 2-3 transitions = 20-30 seconds per plugin
**UX improvement**: Seamless workflow feels automated vs manual checklist

---

## Conclusion

The audit identified 2 routing issues that break seamless workflow progression:

1. **plugin-planning** missing delegation handler (CRITICAL)
2. **plugin-ideation** option 3 showing command instead of invoking skill (MODERATE)

All other decision menus (5 out of 7) correctly invoke skills via the Skill tool.

---

## Fixes Applied

**Status**: ✅ **COMPLETE** - All issues fixed and verified

**Files modified**:
1. `.claude/skills/plugin-planning/SKILL.md`
   - **Lines 152-218**: Added complete delegation handler section
   - **Option 1**: Invokes plugin-workflow skill
   - **Option 2**: Reads and displays contract files
   - **Option 3**: Invokes plugin-ideation skill in improvement mode
   - **Option 4**: Invokes deep-research skill
   - **Option 5**: Confirms state saved, exits
   - **Option 6**: Handles custom input with intelligent routing

2. `.claude/skills/plugin-ideation/SKILL.md`
   - **Lines 514-536**: Fixed handlers for quick parameter menu
   - **Option 1**: Invokes plugin-planning skill with draft params
   - **Option 2**: Invokes ui-mockup skill with draft params
   - **Option 3**: Invokes plugin-planning skill, then suggests ui-mockup (parallel workflow)
   - **Option 4**: Collects custom input
   - **Removed**: All references to manual `/plan` and `/dream` commands

**Files updated for consistency**:
1. `.claude/skills/plugin-planning/assets/decision-menu-stage-0.md`
   - Added handler pattern documentation note
   - References SKILL.md delegation rules

2. `.claude/skills/plugin-planning/assets/decision-menu-stage-1.md`
   - Updated option 1 text: "via /implement" → "invokes plugin-workflow"
   - Added handler pattern documentation note

---

## Post-Fix Verification

All verification checks passed:

✅ **plugin-planning/SKILL.md** - Delegation handler present at lines 152-218
✅ **plugin-ideation/SKILL.md** - Option 3 now invokes plugin-planning skill (lines 528-534)
✅ **Pattern consistency** - Matches existing delegation_rule structure (lines 155-159, 189-192)
✅ **Skill invocations** - All use "Invoke Skill tool" pattern with skill name and prompt
✅ **No manual commands** - Zero references to `/implement`, `/plan`, or `/dream` in handlers
✅ **Asset files** - Templates updated to reflect skill invocation pattern

---

## Impact

**Before fixes**:
- User selects "Start implementation" → Sees "Run: /implement [PluginName]"
- Manual command execution required: 2-3 times per workflow
- Workflow feels like a manual checklist

**After fixes**:
- User selects "Start implementation" → plugin-workflow skill invoked automatically
- Zero manual command execution required
- Workflow feels seamless and automated

**Time savings**: ~20-30 seconds per plugin (2-3 manual command invocations eliminated)
**UX improvement**: Seamless workflow progression (automated vs manual checklist)
