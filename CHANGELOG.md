# Changelog

This documents changes to the CD++ kernel as it was released under codename [Lopez](https://github.com/SimulationEverywhere/CDPP_AdvancedRules-codename-Lopez).

## 4.1.3 - September 2019

### Added

* Instructions for compiling and setting up the introductory [pulse example](examples/pulse).
* Basic Python notebook to post-process `pulse` simulation output and instructions to run it.

### Changed

* Minor improvements in input filename handling in the [tuple_to_real script](scripts/tuple_to_real.py).

## 4.1.2 - December 2018

### Added

* Support for `macOS`.
* Debugging enhancements for the `pulse` introductory example.

### Changed

* Improvements in command-line argument parsing (options and their arguments can be separated by an arbitrary number of spaces).

## 4.1.1 - August 2018

### Added

* New built-in atomic models:
  - Transducer, CPU and queue.

## 4.1.0 - January 2018

### Added

* Some atomic models are now offered as built-in models:
  - A random number generator, parametrized by the desired distribution.
  - Part of the family of QSS numerical integrators --in particular, 
    QSS1, QSS2 and QSS3.

### Changed

* Time management was improved by merging into the simulator some changes to
  the VTime class that provide a more granular definition of time objects
  (i.e., with a less than 1 ms resolution).

## 4.0.1 - November 2017

### Fixed

* Inertial delay was not working as expected. This bug was not introduced by 
  the updates of version 2.0.0 described below. 

## 4.0.0 - March 2017

### Added

* Tuples (i.e., sequences `[v_1,...,v_k]`) can be used as values.
  - This required an update of the CD++ language grammar and
    the definition of new C++ classes to support abstract values.
  - In turn, this allows for a more straightforward way of introducing
    further new values in the future.
  - A Python script that processes a text file and replaces every tuple
    with one of its components (whose index can be supplied by the user)
    was included. This is useful, for example, to post-process a log file
    before feeding it to an external viewer.

### Changed

* Generalization of values exchanged by models and cells (see above).
* Cleaner and more organized source code structure, which included:
  - Depuration of obsolete and/or non-referenced source files.
  - Grouping files into modules according to their functionality.
  - Introducing a brand new building process based on recursive
    Makefiles and dependency generation through GCC, avoiding obsolete
    tools such as makedepend.
* Decoupling of atomic model declaration. It is no longer needed to
  explicitly modify the simulator kernel in order to register a custom
  user-defined atomic model.
