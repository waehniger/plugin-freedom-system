# Stage 2: Foundation

**Context:** This file is part of the plugin-workflow skill.
**Invoked by:** Main workflow dispatcher after Stage 1 completion
**Purpose:** Create plugin structure that compiles successfully with JUCE build system

---

**Goal:** Create plugin structure that compiles

**Duration:** 5 minutes

**Preconditions:**

- Stage 1 complete (plan.md exists)
- creative-brief.md exists
- architecture.md exists

## Actions

### 1. Prepare Contracts for Subagent

Read contract files that foundation-agent needs:

```bash
cat plugins/[PluginName]/.ideas/creative-brief.md
cat plugins/[PluginName]/.ideas/architecture.md
cat plugins/[PluginName]/.ideas/plan.md
```

**CRITICAL: Read Required Patterns**

Read JUCE 8 critical patterns file that MUST be followed:

```typescript
const criticalPatterns = await Read({
  file_path: "troubleshooting/patterns/juce8-critical-patterns.md"
});
```

### 2. Invoke foundation-agent via Task Tool

Call foundation-agent subagent with complete specification:

```typescript
const foundationResult = Task({
  subagent_type: "foundation-agent",
  description: `Create build system for ${pluginName}`,
  prompt: `CRITICAL PATTERNS (MUST FOLLOW):

${criticalPatterns}

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Create foundation build for plugin at plugins/${pluginName}.

Inputs:
- creative-brief.md: plugins/${pluginName}/.ideas/creative-brief.md
- architecture.md: plugins/${pluginName}/.ideas/architecture.md
- plan.md: plugins/${pluginName}/.ideas/plan.md
- Plugin name: ${pluginName}

Tasks:
1. Read creative-brief.md and extract PRODUCT_NAME
2. Read architecture.md and determine plugin type (Audio Effect | Synth | Utility)
3. Create CMakeLists.txt at plugins/${pluginName}/CMakeLists.txt with JUCE 8 integration
4. Create Source/PluginProcessor.h at plugins/${pluginName}/Source/PluginProcessor.h
5. Create Source/PluginProcessor.cpp at plugins/${pluginName}/Source/PluginProcessor.cpp
6. Create Source/PluginEditor.h at plugins/${pluginName}/Source/PluginEditor.h
7. Create Source/PluginEditor.cpp at plugins/${pluginName}/Source/PluginEditor.cpp
8. Return JSON report with status and file list

Build verification handled by workflow after agent completes.
  `,
});
```

### 3. Parse JSON Report with Error Handling

**Implement robust JSON parsing:**

````typescript
function parseSubagentReport(rawOutput: string): object | null {
  try {
    // Strategy 1: Extract from markdown code blocks
    const codeBlockMatch = rawOutput.match(/```json\n([\s\S]*?)\n```/);
    if (codeBlockMatch) {
      return JSON.parse(codeBlockMatch[1]);
    }

    // Strategy 2: Find JSON object via brace matching
    const braceStart = rawOutput.indexOf("{");
    const braceEnd = rawOutput.lastIndexOf("}");
    if (braceStart !== -1 && braceEnd !== -1 && braceEnd > braceStart) {
      const jsonCandidate = rawOutput.substring(braceStart, braceEnd + 1);
      return JSON.parse(jsonCandidate);
    }

    // Strategy 3: Try parsing entire output
    return JSON.parse(rawOutput);
  } catch (error) {
    console.error("JSON parsing failed:", error);
    return null;
  }
}

const report = parseSubagentReport(foundationResult);

if (!report) {
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 2 Error: Could not parse foundation-agent report
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

The foundation-agent subagent completed but returned malformed output.

Raw output:
${foundationResult}

What would you like to do?
1. Retry foundation-agent dispatch
2. Show full subagent output
3. Report bug (subagent should return valid JSON)
4. Manual intervention (I'll fix and say "resume automation")

Choose (1-4): _
  `);

  // Wait for user response, handle accordingly
  return;
}
````

### 4. Validate Required Fields

**Check JSON structure:**

```typescript
function validateFoundationReport(report: any): {
  valid: boolean;
  error?: string;
} {
  if (!report.agent || report.agent !== "foundation-agent") {
    return { valid: false, error: "Missing or wrong 'agent' field" };
  }

  if (!report.status || !["success", "failure"].includes(report.status)) {
    return { valid: false, error: "Missing or invalid 'status' field" };
  }

  if (!report.outputs || typeof report.outputs !== "object") {
    return { valid: false, error: "Missing 'outputs' object" };
  }

  if (!report.hasOwnProperty("ready_for_next_stage")) {
    return { valid: false, error: "Missing 'ready_for_next_stage' field" };
  }

  return { valid: true };
}

const validation = validateFoundationReport(report);
if (!validation.valid) {
  console.log(`
✗ Invalid report format: ${validation.error}

Report received:
${JSON.stringify(report, null, 2)}

What would you like to do?
1. Retry with fresh foundation-agent
2. Report bug (malformed JSON structure)
3. Manual intervention

Choose (1-3): _
  `);
  return;
}
```

### 5. Handle foundation-agent Success/Failure

**If status="success" (files created):**

```typescript
if (report.status === "success" && report.ready_for_next_stage) {
  console.log(`✓ foundation-agent complete: Source files created`);
  console.log(`  - Plugin: ${report.outputs.plugin_name}`);
  console.log(`  - Files: ${report.outputs.source_files_created?.length || 0}`);

  // Now invoke build-automation to verify compilation
  console.log(`\nInvoking build-automation to verify compilation...`);

  // Proceed to section 5a (Build Verification)
}
```

**If status="failure" (file creation failed):**

Present 4-option failure menu with investigation options.

### 5a. Build Verification via build-automation

**CRITICAL: foundation-agent does NOT verify builds. plugin-workflow does this via build-automation.**

**After foundation-agent returns success, invoke build-automation skill:**

```typescript
console.log("✓ foundation-agent complete: Source files created");
console.log("  Files:", report.outputs.source_files_created);
console.log("");
console.log("Invoking build-automation to verify compilation...");

Skill({
  skill: "build-automation",
  context: {
    plugin_name: pluginName,
    stage: 2,
    flags: ["--no-install"],
    invoking_skill: "plugin-workflow",
    purpose: "Verify Stage 2 foundation compiles successfully",
  },
});
```

**build-automation workflow:**

1. Validates plugin directory exists: `plugins/${pluginName}`
2. Validates CMakeLists.txt exists: `plugins/${pluginName}/CMakeLists.txt`
3. Runs build from root: `cd ~/Developer/plugin-freedom-system`
4. Executes: `./scripts/build-and-install.sh ${pluginName} --no-install`
5. Build script runs cmake from root (not plugin directory):
   - `cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release` (if build/ doesn't exist)
   - `cmake --build build --target ${pluginName}_VST3 --target ${pluginName}_AU`
6. If build succeeds: Display success message, return control to plugin-workflow
7. If build fails: Present 4-option failure protocol

**Build failure protocol (handled by build-automation):**
1. Investigate - Run troubleshooter agent
2. Show build log - Display last 100 lines
3. Show code - Display source files with errors
4. Wait - User fixes manually, says "retry build"

**Build directory location:**
- ✅ Correct: `~/Developer/plugin-freedom-system/build/`
- ❌ Wrong: `~/Developer/plugin-freedom-system/plugins/${pluginName}/build/`

**Why foundation-agent doesn't verify builds:**
- Runs in fresh context with limited tools (no Bash)
- Doesn't know build system architecture
- Would require understanding monorepo structure
- Separation of concerns: agents create files, build-automation compiles

**Note:** Build failures are handled entirely by build-automation skill. plugin-workflow just invokes it and awaits result.

### 6. Update State Files

```typescript
updateHandoff(
  pluginName,
  2,
  "Stage 2: Foundation - Build system created, compilation verified",
  [
    "Stage 3: Implement parameters",
    "Review build artifacts",
    "Test compilation",
  ],
  complexityScore,
  phased
);

updatePluginStatus(pluginName, "🚧 Stage 2");
updatePluginTimeline(
  pluginName,
  2,
  "Foundation complete - build system operational"
);
```

### 7. Git Commit

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/CMakeLists.txt
git add plugins/[PluginName]/.continue-here.md
git add PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 2 - foundation

Build system created with JUCE 8 configuration
Minimal PluginProcessor and PluginEditor classes
Compilation verified (VST3, AU, Standalone)

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

### 8. Decision Menu

```
✓ Stage 2 complete: Build system operational

Plugin: [PluginName]
Build artifacts: VST3, AU, Standalone
Status: Compiles successfully (no DSP yet)

What's next?
1. Continue to Stage 3 (implement parameters) (recommended)
2. Review build artifacts
3. Test compilation manually
4. Review Stage 2 code
5. Pause here
6. Other

Choose (1-6): _
```

**Handle responses:**

- 1 or "continue": Proceed to Stage 3
- 2: Show build artifacts list
- 3: Provide manual test instructions
- 4: Display CMakeLists.txt and source files
- 5 or "pause": Update handoff, exit
- 6 or "other": Ask "What would you like to do?" then re-present menu

---

**Return to:** Main plugin-workflow orchestration in `SKILL.md`
