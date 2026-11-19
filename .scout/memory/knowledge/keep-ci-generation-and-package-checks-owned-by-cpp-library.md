---
title: "Keep CI generation and package checks owned by cpp-library"
entity_type: decision
confidence: 1.00
created: 2026-11-19T22:20:43Z
last_accessed: 2026-11-19T22:20:43Z
last_reinforced: 2026-11-19T22:20:43Z
access_count: 1
sources:
  - ".github/workflows/ci.yml:1-4"
  - ".github/workflows/ci.yml:78-80"
  - "User input: \"the CI yaml is intended to come from cpp-library without modification or it will be overridden. And the shorthand for for CPMAddPackage() is intended.\""
tags: ["cpp-library", "generated-ci", "packaging", "cpm"]
tier: knowledge
source: user
---

# Keep CI generation and package checks owned by cpp-library

Treat .github/workflows/ci.yml as generated output: fix its template or shared packaging checks in stlab/cpp-library and regenerate, rather than hand-editing the client workflow. Preserve intended CPMAddPackage shorthand; do not expand dependency declarations merely to change installation eligibility.

Use cpp-library's generated package-artifact check for packaging validation. Do not add or run project-specific installed-package consumption tests beyond cpp-library's provided checks (user-wide rule).

A successful documentation build does not prove Pages publication. Diagnose build, artifact upload, and deployment separately; published-release docs behavior belongs to the upstream workflow template. Distinguish generated find_dependency metadata from physically installing dependency packages.
