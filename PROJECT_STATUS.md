# Project Closure

Date: `2026-05-23`

Status: `closed / archived`

## Outcome

The Term50 fork reached a stable archival state:

- source tree preserved
- build path documented
- packaging path documented
- practical BB10 deployment limits documented

## Why the Project Is Closed

The remaining blocker is platform distribution, not source reconstruction.

Specifically:

- the fork can be rebuilt
- the fork can be packaged
- but the BB10 ecosystem no longer provides a realistic path for a fresh broadly installable signed native release from this repository

## Recommended User Path

For actual installation on devices, use the signed BerryFarm Term49 release when possible:

- `https://github.com/BerryFarm/Term49/releases`

For historical setup/sideload failures such as incomplete post-shutdown onboarding, review:

- `https://lunarproject.org/archive/BlackBerry%2010/OS%20Installs/%5BFix%5D%20BB10%20Devices%20Stuck%20at%20Setup_Sideloading%20Issue.pdf`

## Repository Purpose After Closure

This repo remains valuable as:

- a preservation fork
- a documentation bundle
- a rebuild reference
- a base for private experimentation on legacy BB10 hardware
