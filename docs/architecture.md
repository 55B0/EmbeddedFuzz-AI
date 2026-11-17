# Proposed architecture

**Status:** Phase 0 design. No runtime components are implemented.

## Smallest useful system

A host-based C executable will model a small device's binary request parser. Each test case is one request, processed by a fresh process with deterministic defaults. This trades process startup overhead for straightforward reset and replay semantics. It does not emulate an MCU, peripherals, interrupts, or firmware timing.

The reference target is intended to conform to the [protocol proposal](protocol-design.md). Hand-reviewed specification examples and valid, invalid, and boundary tests will establish confidence; agreement with the reference alone is not proof of correctness. A separate challenge build will introduce individually selectable synthetic faults. The reference build must exclude these faults, and tests must demonstrate this separation.

## Responsibilities

| Component | Responsibility | Boundary |
| --- | --- | --- |
| C targets | Parse one request and emit a bounded binary response | Local child process; no hardware or network access required |
| Python harness | Admit byte inputs, enforce limits, launch the selected target, capture observations | Sole execution path for all generators |
| Conventional generators | Random, mutation-based, boundary-value, and protocol-aware generation | Produce candidates; never launch targets directly |
| LangGraph `StateGraph` | Propose candidates and adapt to permitted feedback | One workflow with externally enforced budgets |
| Deterministic tools | Validate response schema, encode bytes, execute tests, classify observations | No generated code or shell commands |
| Reporting and replay | Re-execute saved bytes and derive metrics from raw records | Model explanations never determine official results |

## Input, feedback, and artifact flow

1. A run configuration fixes generator, target build, seeds, feedback policy, and budgets.
2. A generator proposes bytes and provenance. The AI response must match a strict schema, initially an array of integers from 0 to 255. A shared optional encoder produces valid frames; strategies may also submit raw malformed bytes.
3. Admission checks the response format, byte ranges, input size, and remaining budget. It does **not** require valid protocol fields. Invalid lengths, versions, and commands are test inputs.
4. The harness saves accepted bytes and launches the configured target with a fixed argument list, no shell, and bounded input through standard input. Standard output carries the protocol response; standard error carries diagnostics.
5. It records exit status, timing, timeout or imposed-limit events, bounded output, and compatible coverage if enabled. Truncation is recorded explicitly.
6. Permitted feedback returns to the generator. AI feedback contains bounded execution facts; model interpretations remain separate. Diagnostic paths and source excerpts are excluded from generator feedback under the source-blind study policy.
7. Raw records feed replay and evaluation. Replay and fault attribution occur outside the generation loop under the [evaluation rules](research-questions.md).

Exact inputs must be retained, not just hashes. Hashes support duplicate detection. Run artifacts also capture configuration, target and toolchain identity, prompts, model responses, and budget counters, with secrets excluded. The initial harness should use local files; no database or service is needed.

## Bounded execution

The harness owns target-execution and elapsed-time counters. A deterministic controller bounds model requests, response size, retries, workflow steps, tokens or cost where supported, and time spent waiting for a provider. It reserves limits before dispatch and stops when a cap is reached. Actual usage and overruns must be recorded; estimated cost is not a guaranteed billing limit. The model cannot extend its own budget.

Set a per-process timeout, maximum input size, output cap, artifact quota, and process cleanup policy. Record memory-limit support for the selected platform before benchmarking. A subprocess timeout is not a security sandbox: these experiments use only purpose-built local synthetic targets and do not claim safe execution of arbitrary programs.

Keep credentials in local environment configuration. Never expose credentials in prompts, saved provider headers, or artifacts. The generator must not have filesystem or network tools that can retrieve target source or evaluator-only files.

## Deferred decisions

Recommend a Linux host or WSL with a C compiler and sanitizers as the initial reproducible environment; document the actual toolchain in Phase 1. Compiler versions, coverage instrumentation, and model provider remain unselected. Coverage measurement must use the same instrumentation within a comparison.

Defer persistent processes, stateful frame sequences, real firmware, hardware, multiple agents, and external services. These change reset semantics, execution boundaries, or experimental variables and should follow a working local study.
