---
title: "Keep source-only CI generation owned by cpp-library"
entity_type: decision
confidence: 1.00
created: 2026-11-19T22:20:43Z
last_accessed: 2026-11-19T22:20:43Z
last_reinforced: 2026-11-19T22:20:43Z
access_count: 1
sources:
  - ".github/workflows/ci.yml:1-4"
  - "CMakeLists.txt:4"
  - "User input: \"the CI yaml is intended to come from cpp-library without modification or it will be overridden. And the shorthand for for CPMAddPackage() is intended.\""
tags: ["cpp-library", "generated-ci", "packaging", "cpm"]
tier: knowledge
source: user
---

# Keep source-only CI generation owned by cpp-library

Treat .github/workflows/ci.yml as generated output: fix its template or shared source-build checks in stlab/cpp-library and regenerate, rather than hand-editing the client workflow. Preserve intended CPMAddPackage shorthand; do not expand dependency declarations merely to change installation eligibility.

cpp-library 6.2.0 generates source-only CI. Do not add or run installation, installed-consumer, or package-artifact tests, including any inherited toolkit checks. Retain the opt-in install preset without exercising it (user-wide rule).

A successful documentation build does not prove Pages publication. Diagnose build, artifact upload, and deployment separately; published-release docs behavior belongs to the upstream workflow template. Distinguish generated find_dependency metadata from physically installing dependency packages.
