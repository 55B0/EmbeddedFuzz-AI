# EmbeddedFuzz-AI

*Exploring AI-Assisted Protocol Fuzzing for Embedded Systems*

> **Status: Phase 1 complete.** The C reference target and three challenge variants passed conformance checks on Windows with Visual Studio 2022 Build Tools.

Embedded software often accepts compact binary messages that must be parsed safely despite malformed or unexpected input. Fuzzing can exercise these parsers, but different test generation strategies may reach different behaviors at different costs.

**Research question:** Under controlled execution and resource budgets, can feedback-driven AI test generation improve discovery of reproducible failures in local synthetic protocol targets compared with random, mutation-based, boundary-value, and deterministic protocol-aware generation?

Improvement is not an assumed outcome; negative and inconclusive results are useful. The project is also a practical learning path through C parsing, Python tooling, experimental design, and bounded AI workflows.

## Research objectives

- Build a host-based C protocol target modeling a small embedded device, with a reference target intended to conform to its specification and a separate intentionally vulnerable challenge target.
- Run all generators against the same target through one bounded execution harness.
- Compare random, mutation-based, boundary-value, protocol-aware, and AI-assisted generation under both equal test-count and equal wall-clock or resource budgets.
- Report reproducible failures, known synthetic bugs found, coverage where supported, runtime, duplicate inputs, and LLM usage or cost without conflating measured facts with model interpretations.

The [research questions](docs/research-questions.md) define the proposed comparisons and their limits.

## Proposed architecture

```mermaid
flowchart LR
    A[Random, mutation, boundary, and protocol-aware generators] --> H[Shared Python execution harness]
    L[LangGraph StateGraph] --> V[Structured input validation]
    V --> H
    H --> T[Local C target]
    T --> H
    H --> E[Shared measurements and reporting]
    H --> F[Bounded execution feedback]
    F --> L
```

LangGraph is proposed for planning and adaptation. Deterministic Python code will validate response formats and execution limits while allowing deliberately malformed protocol bytes to reach the target. Model output will be treated as untrusted data; model-generated code will not be executed. The [architecture document](docs/architecture.md) explains component boundaries and planned safeguards.

| Technology | Role | Current state |
| --- | --- | --- |
| Markdown and Mermaid | Project documentation and proposed architecture | In use |
| C | Local synthetic target | Implemented and tested on Windows |
| Python | Shared harness, baselines, evaluation | Planned |
| LangGraph `StateGraph` | Bounded AI-assisted test generation | Planned |
| Coverage tooling and an LLM provider | Instrumentation and optional agent inference | To be selected |

## Roadmap

- [x] **Phase 0:** Scope, proposed architecture, research plan, and public documentation.
- [x] **Phase 1:** Documented binary protocol and local C reference and challenge targets.
- [ ] **Phase 2:** Shared execution harness, conventional baselines, saved inputs, and basic replay.
- [ ] **Phase 3:** Bounded LangGraph agent with structured output and mock-model tests.
- [ ] **Phase 4:** Coverage guidance, failure minimization, and improved deduplication.
- [ ] **Phase 5:** Reproducible benchmarking and comparison.
- [ ] **Phase 6:** Optional hardware integration.

Milestones and acceptance criteria are in the [development roadmap](docs/development-roadmap.md). The [protocol specification](docs/protocol-design.md) defines the Phase 1 wire format.

## Getting started

On Windows, open the **x64 Native Tools Command Prompt for VS 2022** with the C++ Build Tools and Windows SDK installed. Run from the repository root:

```bat
if not exist build mkdir build
cl /nologo /std:c11 /W4 /WX /Fobuild\reference.obj /Febuild\reference.exe src\target.c
for %i in (1 2 3) do cl /nologo /std:c11 /W4 /WX /DEF_FAULT=%i /Fobuild\fault-%i.obj /Febuild\fault-%i.exe src\target.c
python tests/test_target.py build\reference.exe build\fault-1.exe build\fault-2.exe build\fault-3.exe
```

On Linux or WSL, use a C11 compiler and Python 3. Run from the repository root:

```sh
mkdir -p build
cc -std=c11 -Wall -Wextra -Wpedantic -Werror src/target.c -o build/reference
for fault in 1 2 3; do
    cc -std=c11 -Wall -Wextra -Wpedantic -Werror -DEF_FAULT="$fault" src/target.c -o "build/fault-$fault" || exit 1
done
python3 tests/test_target.py build/reference build/fault-1 build/fault-2 build/fault-3
```

Try a binary `PING`:

```sh
printf '\105\106\001\001\000\000' | ./build/reference | od -An -tx1
```

The expected output is `00 00 00`. The [architecture](docs/architecture.md) and [evaluation plan](docs/research-questions.md) explain how later phases will use these targets.

## Limits and future work

The initial proposal uses one request per process and three commands without a checksum. It models parser behavior, not an MCU, peripherals, firmware timing, or interrupts. Synthetic faults and educational baselines limit what results can establish about real firmware and mature coverage-guided fuzzers. Coverage instrumentation and model variability also affect comparisons; see the [evaluation plan](docs/research-questions.md). Stateful sequences and hardware integration are future work after a reproducible local comparison exists.

Contributions should follow [CONTRIBUTING.md](CONTRIBUTING.md). This project is licensed under the [MIT License](LICENSE).
