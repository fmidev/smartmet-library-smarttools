# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

`smartmet-library-smarttools` is a C++17 shared library (`libsmartmet-smarttools.so`) providing the scripting engine and data management layer for the SmartMet Editor (Windows workstation) and `smartmet-qdtools` command-line tools. It is part of the SmartMet Server ecosystem developed by the Finnish Meteorological Institute (FMI).

## Build Commands

```bash
make                # Build the shared library
make clean          # Clean build artifacts
make format         # Run clang-format on all source files
make rpm            # Build RPM package (cleans first)
make install        # Install headers to /usr/include/smartmet/smarttools/ and .so to libdir
```

There is no test suite in this repository — no `test/` directory exists.

## Dependencies

- **smartmet-library-newbase** — heavily used; provides `NFmiAreaMask*`, `NFmiFastQueryInfo`, `NFmiQueryData`, `NFmiArea`, `NFmiParam`, `NFmiLevel`, `NFmiProducer`, `NFmiMetTime`, `NFmiDataMatrix`
- **smartmet-library-macgyver** and **smartmet-library-gis** — build-time dependencies
- **Boost** (regex, iostreams, thread), **fmt** (12.x)

## Architecture

All source lives in `smarttools/` (48 headers, 46 cpp files). There are five main subsystems:

### SmartTool Scripting Language

A domain-specific language for meteorological calculations with an interpreter-executor split:

- **`NFmiSmartToolIntepreter`** — parses script text into a structured representation of calculation blocks, conditions, and operations. Supports `IF/ELSEIF/ELSE` blocks, variable declarations (`var`/`const`), vertical functions (`vertp_max`, etc.), met functions (advection), math functions, peek functions (time offset), and ramp functions.
- **`NFmiSmartToolModifier`** — executes parsed scripts against actual data, converting parsed info into runtime `NFmiSmartToolCalculation` and `NFmiSmartToolCalculationBlock` objects.
- **`NFmiSmartToolCalculation`** — handles a single calculation line (e.g. `T = T + 1`).
- **`NFmiSmartToolCalculationSection`** — groups calculations within a block.
- **`NFmiSmartToolUtil`** — static utility interface for script execution.
- MacroParam scripts use `RESULT = ???` syntax for derived parameters.

### Data Management

- **`NFmiInfoOrganizer`** — central registry and single point of access for all loaded query data. Thread-safe via `NFmiQueryDataKeeper`. Provides specialized accessors for sounding data, macro-param grids, cross-section data, and time-serial data.
- **`NFmiOwnerInfo`** — shared-pointer wrapper around `NFmiFastQueryInfo` with data ownership tracking.
- **`NFmiSmartInfo`** — extends `NFmiOwnerInfo` with undo/redo and masking support.
- **`NFmiQueryDataKeeper`** — manages query data lifecycle with thread-safe access.
- **`NFmiProducerSystem`** — registry of available data producers (weather models).

### Mask & Condition System

Extends newbase's `NFmiAreaMask` framework for conditional operations on grid data:

- **`NFmiAreaMaskInfo`** / **`NFmiAreaMaskSectionInfo`** — metadata for area masks and mask groups.
- **`NFmiSimpleConditionInfo`** — simple conditional expressions (e.g. `WS_ec > 10`).
- **`NFmiMultiLevelMask`** / **`NFmiUndoableMultiLevelMask`** — multi-level selection masks with undo.
- **`NFmiLocalAreaMinMaxMask`** — local extrema detection (note: disabled via `DEBUG_LOCAL_EXTREMES` flag due to compiler issues with `std::async`).
- **`NFmiStation2GridMask`** / **`NFmiInfoAreaMaskOccurrance`** / **`NFmiInfoAreaMaskSoundingIndex`** — specialized mask types.

### Sounding (Vertical Profile) Subsystem

- **`NFmiSoundingData`** — container for vertical profile data (pressure levels with T, Td, RH, WS, WD). Supports multiple LCL calculation modes (Surface, 500m, MostUnstable).
- **`NFmiSoundingFunctions`** — pure functions for thermodynamic calculations (theta-e, mixing ratio, dew point, LCL).
- **`NFmiSoundingIndexCalculator`** — computes stability indices (CAPE, CIN, LCL, LFC, EL, SHOW, LIFT, KINX, CTOT, VTOT, TOTL, GDI) and wind shear indices (Bulk Shear, Storm-Relative Helicity). Multi-threaded.
- **`NFmiTEMPCode`** — decodes TEMP upper-air observation messages (ABCD sections).
- **`NFmiRawTempStationInfoSystem`** / **`NFmiAviationStationInfoSystem`** — station metadata for TEMP observations and aviation stations (ICAO/WMO).

### Drawing & Visualization Parameters

Used by the SmartMet Editor workstation for rendering configuration:

- **`NFmiDrawParam`** — visualization metadata: colors, isoline settings, symbol properties, contour settings, view type. Supports forward/backward compatible serialization.
- **`NFmiDrawParamFactory`** — creates DrawParam instances from data identity + level.
- **`NFmiDrawParamList`** — ordered collection of draw parameters.
- **`NFmiColor`** / **`NFmiColorSpaces`** / **`FmiColorTypes.h`** — RGBA colors with RGB/HSL conversion, blending, and alpha operations.

## Key Patterns

- **Shared-pointer ownership**: methods return `shared_ptr` copies for thread-safe concurrent access.
- **Info copying**: `NFmiInfoOrganizer` returns cloned info objects so multiple threads can iterate independently.
- **Windows path compatibility**: `NFmiPathUtils` handles case-insensitive paths, mixed separators (`\` and `/`), drive letters, and UNC paths for cross-platform SmartMet Editor support.

## Code Style

Configured in `.clang-format`: Google-based style, Allman braces, 100-column limit, no bin-packing of arguments/parameters. Run `make format` before committing.

## CI

CircleCI builds RPMs on RHEL 8 and RHEL 10 (`fmidev/smartmet-cibase-{8,10}` images) and uploads to S3 (`s3://fmi-smartmet-cicd-beta/`).
