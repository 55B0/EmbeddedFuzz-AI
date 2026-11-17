# Research questions and evaluation plan

**Status:** Proposed methodology. No experiments or measurements exist.

## Questions and minimum study

Under controlled execution and resource budgets, can feedback-driven AI test generation improve discovery of reproducible failures in local synthetic protocol targets compared with random, mutation-based, boundary-value, and deterministic protocol-aware generation?

The first study will use the [single-frame protocol](protocol-design.md), three proposed fault variants, and six generation conditions: four educational baselines, AI with execution feedback, and the same AI workflow without execution feedback. Each condition will test each challenge variant under a fixed configuration. Keep the reference target available for conformance checks; it is not an independently proven oracle.

The full comparison evaluates complete strategies. The limited feedback ablation asks whether feedback helps the same model, prompt scaffold, tools, and budgets. Do not infer a general AI advantage from either result. Do not generalize these baselines to mature coverage-guided fuzzers.

Primary outcomes are **distinct known bugs reproduced** and **elapsed time and target executions to first discovery**. Report bugs not found as not found within the budget, not as zero discovery time. Supporting metrics include failure counts, coverage, throughput, duplicate inputs, rejected model responses, and model usage.

## Generator information and fairness

| Condition | Generation rule | Permitted information |
| --- | --- | --- |
| Random | Sample lengths and raw bytes from a fixed documented distribution | Common size limits; intentionally ignores protocol knowledge |
| Mutation | Fixed seeded byte mutations of the common initial corpus | Common corpus and limits; no adaptive outcome selection initially |
| Boundary-value | Deterministic enumeration around field and size boundaries | Public protocol, common corpus, shared encoder |
| Protocol-aware | Deterministic command and malformed-field enumeration | Same public protocol, corpus, and encoder |
| AI with feedback | Structured candidates adapted from bounded observations | Same protocol, corpus, encoder, and execution outcomes |
| AI without feedback | Same workflow with execution observations withheld | Same protocol, corpus, encoder; own prior outputs and remaining budgets |

Freeze and record enumeration order, random distributions, mutation operators, prompts, corpus, and limits before measured runs. The corpus contains reviewed normal examples, not challenge triggers. Helpers and raw-byte submission are available through the same interface; record which strategies actually use them.

The full strategy comparison deliberately includes different uses of protocol knowledge and feedback. This is not a controlled estimate of the model's effect alone. The AI feedback ablation isolates only feedback access, subject to model variation. Withholding feedback must also remove crash-derived corpus updates and other indirect feedback.

Initially, coverage may be collected for evaluation but is withheld from all generators. Later coverage-guidance experiments must document access for every condition; giving coverage only to AI changes both information and strategy.

All conditions use the same target binary, harness, limits, initial corpus, and instrumentation within a comparison. No generator receives source code, the fault manifest, evaluation triggers, or raw source-bearing diagnostics. Exclude source paths and lines from feedback; use bounded status categories and measured facts instead.

## Failure definitions and attribution

| Observation | Treatment |
| --- | --- |
| Specification-compliant rejection | Expected behavior, not a bug |
| Crash or sanitizer report | Suspected target failure requiring replay |
| Per-test timeout | Suspected failure; distinguish target behavior from host overload or an imposed run deadline |
| Response contradicting the specification | Suspected semantic failure using explicit deterministic checks |
| Launch, storage, provider, or harness failure | Infrastructure failure; excluded from target bug counts |
| Schema or input-limit rejection | Candidate admission failure; target was not executed |

Plan three separately enabled synthetic fault variants with identifiers such as `F001` through `F003`. Each evaluator-only manifest entry will describe its source change, fault family, trigger bytes, expected symptom, and reference behavior. Test that enabling one fault does not silently enable another, and that the reference build excludes all injected faults.

Recommend two additional replays of each suspected failure in fresh processes with the same target, limits, and input. Count it as reproducible only if both repeat the classified symptom. Report intermittent cases separately. This is a small-study rule, not proof of determinism.

A distinct crashing input is not necessarily a distinct bug. Attribute known bugs using the variant, manifest, and reproduced symptom; do not automatically attribute every failure on a variant to its injected fault. Unmatched failures remain unclassified pending review. Stable symptom signatures help grouping but do not prove root-cause identity. Model explanations never assign official fault IDs.

Public source can still leak fault knowledge through model training or manual prompt preparation. Keep evaluator files out of runtime context, record model and prompt provenance, and acknowledge that a public project cannot guarantee absence of prior exposure.

## Budget accounting

Run separate equal-execution and equal-elapsed-time experiments. Apply a documented model-call and token or cost ceiling to AI conditions, plus a generous finite elapsed-time safety ceiling to execution-budget runs. Record early stops rather than representing unequal completed runs as equal-budget results. Choose numerical budgets after a pilot, then freeze them before the reported comparison.

| Event | Accounting rule |
| --- | --- |
| Generated candidate | Count each proposed test input, including duplicates; malformed provider responses are counted separately |
| Accepted candidate | Schema-valid bytes within harness limits; protocol validity is irrelevant |
| Target execution | Count each dispatched test, including duplicate bytes and timeouts; record launch failures separately |
| Invalid model response or retry | Uses elapsed time, call allowance, and reported usage; consumes no target execution |
| Duplicate input | Execute and charge normally in the initial study; measure duplicates without silent filtering |
| Provider failure | Log and apply a bounded retry policy; record known usage and mark unknown usage |
| Failure replay or minimization | Separate post-run evaluation budget, identical across conditions; do not feed results back into generation |
| Setup and warm-up | Record separately and exclude from search time; no target-informed agent warm-up |
| Online generation and feedback | Include model waiting, validation, serialization, process startup, execution, and recording in search time |

Record generation completion and execution completion separately. At a time deadline, stop issuing work and cancel outstanding work where possible. A test cut off by the run deadline is not a target timeout finding. Do not credit discoveries completed after the search deadline. Post-run replay may confirm an earlier discovery; its cost must still be reported.

## Artifacts and measurements

Retain exact executed bytes, not only digests; generator seed and configuration; target revision and binary identity; build flags and toolchain; relevant dependency versions; resource limits; run conditions; raw outputs and truncation indicators; model identifier, settings, prompts, responses, requests, tokens, and usage estimates. Exclude secrets and provider authorization headers.

Distinguish candidate count, accepted count, search executions, and replay executions. Report both raw reproducible failures and distinct attributed known bugs. Record total search time, target execution time, first-discovery timestamps, duplicate rate with its denominator, and schema rejection counts. If cost is estimated, record the pricing basis and date; unavailable usage is unknown, not zero.

Coverage tooling is unselected. Recommend source branch coverage on the same instrumented build, reporting covered and total branches in the target parser while excluding harness and runtime code. Freeze the denominator and aggregation method before comparison, and reset coverage records between trials. If unavailable, omit the metric explicitly. Sanitizer and coverage overhead must be consistent and disclosed.

## Trials and interpretation

Pilot budget sizes and operational reliability before freezing the measured study. Use multiple documented seeds or trials, preselect their count after the pilot, and vary run order to reduce host-load bias. Keep pilot data distinct from evaluation data. Report individual runs and variation, including early stops, provider failures, and negative findings; do not cherry-pick the best run.

Seeds reproduce deterministic generators, not necessarily hosted model responses. Saved bytes support failure replay even when the full model-driven search cannot be reproduced. Preserve both types of evidence.

Limit conclusions to this protocol, small synthetic fault set, selected baselines, model settings, and budget conditions. Protocol-specific baseline tuning, arbitrary enumeration order, public-source exposure, model variability, instrumentation overhead, and host process costs can bias outcomes. This study does not establish performance on real firmware or hardware.
