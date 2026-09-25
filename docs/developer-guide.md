# smarttools developer guide

This guide is for developers who change `smartmet-library-smarttools`. smarttools implements
the **SmartTool** script language, which meteorologists use to edit and derive QueryData
("smarttools" and "macro parameters"), together with the data bookkeeping, sounding indices
and drawing parameters of the SmartMet Editor. The Windows SmartMet Editor (`workstation`)
and the `qdscript` and `qdsoundingindex` programs of qdtools use it; inside the server, only
the trajectory library uses a small part of it (`NFmiDataStoringHelpers`).

[CLAUDE.md](../CLAUDE.md) has the subsystem overview.

## Contents

1. [Building and testing](#1-building-and-testing)
2. [Running a script](#2-running-a-script)
3. [The script language](#3-the-script-language)
4. [Interpreter and modifier](#4-interpreter-and-modifier)
5. [Data bookkeeping](#5-data-bookkeeping)
6. [Sounding indices](#6-sounding-indices)
7. [Compatibility](#7-compatibility)
8. [Known pitfalls](#8-known-pitfalls)

---

## 1. Building and testing

```bash
make
```

There are no tests in this repository, and qdtools has no `qdscript` regression test
either: the language is tested by running scripts with `qdscript` and in the SmartMet
Editor. The code also builds on Windows (CMake), so keep it portable.

## 2. Running a script

`NFmiSmartToolUtil::ModifyData()` is the entry point for programs:

```cpp
NFmiQueryData* result = NFmiSmartToolUtil::ModifyData(scriptText,
                                                       data.release(),   // ownership passes
                                                       &helperFiles,     // other data files
                                                       false,            // no drawparam files
                                                       goThroughLevels,
                                                       makeStaticIfOneTimeStep);
```

* The data given in is **taken over** by the internal `NFmiInfoOrganizer`, which deletes
  it; the result is a new copy of the edited data, or a null pointer on failure.
* Errors in interpreting or running the script are printed to standard error and give a
  null result instead of an exception.
* The data must already contain every parameter the script assigns to; `qdscript` adds
  missing parameters before calling `ModifyData()`.
* Helper data files become the other data sets that scripts can refer to by producer.
* With `goThroughLevels`, the script is run once for each level of the edited data.

## 3. The script language

A script is a sequence of calculation lines and conditional blocks:

```
T = T + 1                           // runs for every point and time
IF (T_ec < 0 && RR > 0.1)
{
  PREF = 3                          // snow
}
ELSEIF (T_ec < 2)
  PREF = 2
ELSE
  PREF = 1
var x = time_avg(T_ec -3 3)         // user variable
```

* **Comments** (`//`, `/* … */`) are stripped by newbase's `NFmiPreProcessor`, which also
  handles `#include` of other script files from the include directory.
* **Operators**: arithmetic, comparisons (`<`, `<=`, `==`, `!=` or `<>`, …) and `&&`, `||`
  (or `AND`, `OR`) in conditions. Function arguments are separated by spaces, for example
  `time_avg(T_ec -3 3)` for the mean from three hours before to three hours after.
* **Variables** are parameter names, optionally with a producer and a level:
  `T`, `T_ec`, `T_850`, `T_ec_850`, or numeric forms `par4_prod240_850`. Level prefixes
  select the level type: a plain number is a pressure level, `lev` a hybrid level,
  `fl` a flight level and `z` a height. Without a producer, the edited data is used.
* **Blocks**: one `IF`, any number of `ELSEIF`s and one `ELSE`, with the conditions in
  parentheses and the bodies optionally in braces. Calculations before and after the
  blocks always run. Keywords are recognised in upper, lower and capitalised case.
* **User variables**: `var` (per point) and `const`.
* **Macro parameters** assign to `RESULT` instead of a data parameter; the editor draws
  the result.
* **Functions**: mathematics (`sin`, `sqrt`, `round`, …); time and area integration
  (`time_avg`, `area_max`, …, `previousfulldays_*`); vertical functions over pressure,
  heights, hybrid or flight levels (`vertp_max`, `vertz_findh`, …); time-and-vertical
  combinations (`timevertp_avg`, …); peeks at other times and points (`peekt`, `peekxy`);
  gradients, divergence, advection and Laplacians; occurrence and probability functions
  (`occurrence_*`, `probcircle_*`, `probrect_*`); sounding indices (`cape`, `cin`,
  `lcl…`); and topography, land/sea and distance-to-coast values.

The keyword and function tables are built in `NFmiSmartToolIntepreter::InitTokens()`; that
is the reference for the exact names and argument counts.

## 4. Interpreter and modifier

1. `NFmiSmartToolIntepreter` parses the text into `…Info` objects: calculation sections,
   conditional blocks, masks (`NFmiAreaMaskInfo`, `NFmiSimpleConditionInfo`) and their
   operands. Syntax errors throw with a `SmartToolError…` message key.
2. `NFmiSmartToolModifier::InitSmartTool()` turns the infos into runtime objects:
   `NFmiSmartToolCalculationBlock`s of `NFmiSmartToolCalculation`s, with newbase area masks
   (`NFmiAreaMask` and its subclasses) as operands, each bound to an info of the right data
   set from the `NFmiInfoOrganizer`.
3. `ModifyData_ver2()` runs the blocks for every time step and grid point (or only the
   selected points in the editor). Grids are split into blocks processed by several threads
   (`smt-gridblk`), each with cloned infos.

To add a function, add its token in `InitTokens()`, a parse rule in the interpreter, and
an area mask class (usually in `NFmiInfoAreaMask*` in newbase or in this library) that
computes it for one point.

## 5. Data bookkeeping

* `NFmiInfoOrganizer` is the registry of all loaded data, by `NFmiInfoData` type
  (editable, model, observation, …), producer and level type. It returns **clones** of the
  infos, so callers can iterate independently.
* `NFmiOwnerInfo` ties an info to the shared data it came from; `NFmiSmartInfo` adds undo
  and redo, and selection masks, for the editor.
* `NFmiProducerSystem` lists the known producers and their names in scripts.
* `NFmiDrawParam` and friends describe how the editor draws a parameter; their text
  serialisation must stay readable by old and new editor versions.

## 6. Sounding indices

`NFmiSoundingData` holds one vertical profile, and `NFmiSoundingIndexCalculator` computes
stability indices (CAPE, CIN, LCL, LFC, EL, Showalter, lifted, K, total totals, …) and wind
shear indices for a profile, or for a whole grid into new QueryData (`qdsoundingindex`).
`NFmiSoundingFunctions` has the thermodynamic formulas. LCL calculations can start from the
surface, from a 500 m mixed layer or from the most unstable level.

## 7. Compatibility

The headers are installed and used by qdtools and the trajectory library, and the editor
builds the same sources on Windows. The drawparam and data storing formats are read by
existing editor installations: keep them backward and forward compatible
(`NFmiDataStoringHelpers` has the helpers for extra fields). Many classes extend newbase
classes, so a newbase change can require rebuilding this library.

## 8. Known pitfalls

* **`ModifyData()` takes ownership of its input data** (§2).
* **Errors are printed, not thrown**, by `ModifyData()` (§2).
* **The token tables are static and filled on first use without locking.** Interpret the
  first script before starting other threads that interpret scripts.
* **Assigned parameters must exist** in the edited data (§2).
* **Keyword case variants are listed explicitly**; a new keyword needs its variants too.
