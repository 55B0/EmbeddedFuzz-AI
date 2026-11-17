# Development roadmap

Phases 0 and 1 are complete. Phase 1 builds and conformance checks passed on Windows with MSVC 14.44.

## Phase 0 — Foundation

- [x] Scope, proposed architecture, protocol proposal, and research methodology.
- **Learning:** Turn a broad question into a bounded experiment.
- **Outcome:** A documentation-only repository with clear assumptions and limits.
- **Acceptance:** Required documents and license exist; links and status agree; proposed components, experiment controls, and open decisions are explicit.
- **Dependencies:** Repository inspection and the agreed Phase 0 brief.

## Phase 1 — Local C targets

- [x] Finalized initial protocol, reference target, and separate challenge variants.
- **Learning:** Binary parsing, bounds checks, compiler diagnostics, and specification-based tests.
- **Outcome:** One request can be processed locally, with expected success or rejection.
- **Acceptance:** Documented build and test commands work; reviewed valid, invalid, and boundary cases pass on the reference; each challenge variant has an evaluator-only manifest and reproducible trigger; injected faults are absent from the reference build.
- **Dependencies:** [Version 1 protocol specification](protocol-design.md) and a C11 toolchain.

## Phase 2 — Harness, baselines, and replay

- [ ] Shared bounded harness, four conventional generators, and exact-input replay.
- **Learning:** Subprocess control, reproducible generation, outcome classification, and artifact design.
- **Outcome:** A saved baseline input can be replayed against the same target.
- **Acceptance:** All generators use the shared harness; malformed protocol bytes reach the target; timeout and output limits work; seeds and configurations are recorded; exact bytes and outcomes survive replay; candidate and execution counters are distinct.
- **Dependencies:** Phase 1 targets and deterministic expected-response checks.

## Phase 3 — Bounded AI workflow

- [ ] One LangGraph workflow with structured output and mock-model tests.
- **Learning:** Workflow state, schema validation, feedback, and independent budget enforcement.
- **Outcome:** A mock model proposes a case, receives permitted feedback, and stops at a configured limit.
- **Acceptance:** Tests cover malformed output, exhausted budgets, bounded retries, and provider failure; generated code is never executed; the workflow supports feedback-enabled and feedback-withheld conditions without hidden outcome leakage.
- **Dependencies:** Phase 2 harness and stable observation records. Choose a provider only when a live pilot is needed.

## Phase 4 — Feedback and analysis

- [ ] Compatible coverage guidance where supported, failure minimization, and improved deduplication.
- **Learning:** Instrumentation, failure signatures, and root-cause attribution limits.
- **Outcome:** Replay and minimize one known failure; produce a documented duplicate grouping.
- **Acceptance:** Minimization preserves the replayed symptom; attribution rules distinguish known and unclassified failures; coverage numerator, denominator, and feedback-access policies are fixed before comparisons. Missing instrumentation is documented.
- **Dependencies:** Phase 2 replay and Phase 3 feedback controls.

## Phase 5 — Reproducible comparison

- [ ] Pilot, frozen experiment configuration, measured runs, and an honest report.
- **Learning:** Budget design, repeated trials, variation, and restrained interpretation.
- **Outcome:** Compare four baselines and AI with/without feedback under execution and elapsed-time budgets.
- **Acceptance:** Configurations and raw evidence support the report; replay confirms credited discoveries; unknown usage and incomplete runs are visible; primary outcomes and variation are reported alongside negative or inconclusive findings.
- **Dependencies:** Earlier runtime phases and the [evaluation plan](research-questions.md). Exact budgets and trial count follow a pilot.

## Phase 6 — Optional hardware

- [ ] Evaluate a specific device only after the local study is reproducible.
- **Learning:** Transport, reset behavior, timing, and hardware observability.
- **Outcome:** A bounded adapter can replay a documented benign input on a selected device.
- **Acceptance:** Define device-specific limits, reset/recovery, authorization, and observability before execution; publish limitations relative to the local target.
- **Dependencies:** A completed local comparison and a separately chosen hardware scope.

## Recommended next task

Start Phase 2 with a Python harness that runs one saved request against a selected target, enforces a timeout, and records the exact request and observed response so the case can be replayed.
