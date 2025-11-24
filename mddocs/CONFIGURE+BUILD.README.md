# Configuration and building with CMake presets

This project is to be built with CMake, using `CMakeLists.txt` file and 
`CMakePresets.json` file. `CMakePresets.json` is a configuration file that
lives in the project's root directory.  It was introduced with CMake
version 3.19, to share and save common project-wide configuration options,
in order to facilitate sharing them across projects and with others.

When such a new optional configuration file is introduced, next to the
obligatory `CMakeLists.txt` an important question to answer is:

**What is the preferred division of labour between `CMakeLists.txt` and `CMakePresets.json`?**

While there may other preferences and approaches, the answer to that question
given for this project is:

1. `CMakeLists.txt` focuses on *what* must be build, what the targets are
    and which sources are required for each target.

2. `CMakePresets.json` focuses on a presets that describe *how* targets
   are to be built, with particular versions of particular tooling, and
   tooling specific options.

 
- `CMakeLists.txt` typically is for specifying:
  
  1. a minimally required CMake version;
  
  2. required coding standards, such as ISO C++17 or C++20, ISO C11 or C17, that assert:
  
     1. the availability of specific *language* features as prescribed by the standard;
     2. the availability of specific *standard library* features as prescribed by the standard;
  
  3. which targets, executables and/or libraries, to build, and which sources to use for each
     specified target.
  
  The `CMakeLists.txt` file thus should have very little branching that has to do with
  platform and optimization specific switches for the underlying tooling, if any at all. 

- `CMakeLists.json` is for specifying presets, 

  1. for debug and release builds, and set a preset-specific build output directory;

  2. that determine which tooling, i.e. which compiler and/or a linker, to use;

  3. that specify a minimal version for the tooling to be used.

  4. that specify switches to be used for the tooling, since these are in most cases
     very tooling specific.
  
## Summary of commands

- Commands to configure and build in release mode:
	- `cmake --preset release`
	- `cmake --build --preset release`

- Commands to configure and build in debug mode:
	- `cmake --preset debug`
	- `cmake --build --preset debug`

