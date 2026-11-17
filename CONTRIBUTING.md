# Contributing

EmbeddedFuzz-AI is developing its Phase 1 C target. Issues and pull requests that clarify protocol behavior, conformance tests, or the research design are welcome.

Before proposing a change, read the [architecture](docs/architecture.md), [research questions](docs/research-questions.md), and [roadmap](docs/development-roadmap.md). Keep contributions small and explain the reason for each design choice. Label proposed behavior as proposed, and distinguish observations from hypotheses. Do not add benchmark figures without reproducible experiment conditions and data.

For future code contributions, prefer deterministic tests, fixed seeds where applicable, bounded execution, and minimal dependencies. Keep exact input artifacts for local replay; never commit credentials or real device secrets. Generated experiment directories are ignored by default. Any future published evidence should be deliberately reviewed and include its configuration and provenance.

Keep evaluator-only bug manifests and trigger inputs out of generator prompts and accessible tools. Documentation review must not silently become implementation of a later phase. Describe checks accurately in the pull request, distinguishing automated checks, manual review, and unperformed checks. Add runnable commands only once the corresponding functionality exists.

This project is released under the [MIT License](LICENSE).
