# Skill Documentation Templates

Standard sections to add to all SKILL.md files for consistency.

## Decision Menu Protocol Template

Add this section to all skills (customize examples as needed):

```markdown
## Decision Menu Protocol

**Use AskUserQuestion tool ONLY when:**
- In ideation/design phases (exploring creative concepts)
- 2-4 conceptual options (not workflow actions)
- Examples for this skill:
  - [Skill-specific creative decision example]
  - [Skill-specific design choice example]

**Use Inline Numbered Menu for:**
- Checkpoint decision gates (workflow navigation)
- Template/item selection from existing options
- ANY menu with >4 options
- Examples for this skill:
  - [Skill-specific checkpoint example]
  - [Skill-specific selection menu example]

**Key difference:** AskUserQuestion = exploring ideas, Inline Menu = navigating workflow or choosing items

**Inline menu format:**

\```
[Completion statement]

What's next?

1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery option] ← User discovers [feature]
4. [Alternative path]
5. Other

Choose (1-5): _
\```

ALWAYS wait for user response. NEVER auto-proceed.
```

## Integration Contracts Template

Add this section to skills that invoke other skills or are invoked by others:

```markdown
## Integration Contracts

### Invokes: [skill-name]

**When:** [Trigger condition - when this integration happens]

**Sends:**
- [parameter_name]: [type] - [description]
- [parameter_name]: [type] - [description]

**Expects:**
\```json
{
  "field": "value",
  "description": "what the response looks like"
}
\```

**Error handling:**
- [Error scenario] → [How this skill handles it]
- [Error scenario] → [How this skill handles it]

**Contract:** [Key invariant about the integration - who owns what, read-only vs modify, etc.]

---

### Invoked by: [skill-name]

**Receives:**
- [parameter_name]: [type] - [description]

**Returns:**
- [return_value]: [type] - [description]

**Contract:** [Key invariant about what this skill guarantees]

---

### Subagent: [agent-name] (if applicable)

**When:** Stage [N] implementation

**Sends:**
- Plugin name (string)
- Contract files ([list specific contracts])
- Stage directive (string)

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`

See `.claude/docs/integration-contracts.md` for complete subagent report schema.

**Error handling:**
- [Specific error] → [How orchestrator handles it]

**Contract:** [Key invariant - typically: subagent implements, orchestrator validates and commits]
```

## Schema Reference Template

Add this to skills that generate or consume JSON:

```markdown
## Data Formats

### [Format Name]

**Schema:** `.claude/schemas/[schema-name].json`

**Purpose:** [What this format is used for]

**Validation:**

\```python
import json
from jsonschema import validate

with open('.claude/schemas/[schema-name].json') as f:
    schema = json.load(f)

validate(instance=data, schema=schema)
\```

**Required fields:**
- `field_name`: [type] - [description]
- `field_name`: [type] - [description]

**Optional fields:**
- `field_name`: [type] - [description]

See `.claude/schemas/README.md` for validation examples.
```

## Error Handling Template

Add this to all skills:

```markdown
## Error Handling

### Common Errors

**[Error scenario]:**
- **Detection:** [How to detect this error]
- **Recovery:** [What the skill does to recover]
- **User action:** [What user should do, if any]

**[Error scenario]:**
- **Detection:** [How to detect this error]
- **Recovery:** [What the skill does to recover]
- **User action:** [What user should do, if any]

### Graceful Degradation

When [critical dependency] unavailable:
- [Fallback behavior]
- [Alternative path offered to user]
- [How to restore full functionality]

### Error Reporting Format

All errors presented to user include:
1. **What failed** - Clear description of the error
2. **Why it failed** - Root cause (if known)
3. **How to fix** - Actionable steps
4. **Alternative** - Other ways to proceed

Example:
\```
ERROR: [What failed]

Reason: [Why it failed]
Solution: [How to fix it]
Alternative: [Other options]

What would you like to do?
1. [Recovery option]
2. [Alternative path]
3. [Manual intervention]

Choose (1-3): _
\```
```

## Usage

When documenting a skill:

1. **All skills MUST have:**
   - Decision Menu Protocol section
   - Error Handling section

2. **Skills that invoke others MUST have:**
   - Integration Contracts section (for each integration)

3. **Skills that generate/consume JSON MUST have:**
   - Data Formats section (with schema references)

4. **Skills that orchestrate subagents MUST have:**
   - Subagent integration contracts
   - Schema validation references

This ensures consistent, maintainable documentation across all skills.
