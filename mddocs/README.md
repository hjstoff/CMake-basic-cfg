# CMake basic configuration reference system

## ' Snellius', and its current OS distribution, is the primary target and reference system
are the primary reference system for the programming projects that use this setup for
developing and building programs written in C++, and possibly in C, and configured with
CMake.

"Snellius.surf.nl" is the current Dutch national supercomputer for academia.
"Snellius" currently (May/June 2025) is a cluster of GNU/Ĺinux nodes currently configured
with version 9.x of the RHEL operating system distribution. Several of the settings 
stem from restrictions and requirements pertaining tho this reference system.

### Restrictions on versions of the tooling 
One important requirement for the programs built with this project configuration
is that they can be built with the standard tooling and versioning that comes with the
current Snellius OS distribution, and with as little additional, external, dependencies
as possible. In particular, required and specified versions of gcc/g++, cmake, and other
core tooling, must not exceed the versions of these tools distributed with the OS release 
currently installed on Snellius (May 2025: GCC 11.3, CMake 3.20).

For C++ this implies that the highest language standard to be applicable is C++20, as the
GNU tooling distributed with RHEL 9.x has not sufficient support for C++23 yet. For
parallelism, native C++ threads and jthreads as supported by C++20 can be used.
Alternatively, OpenMP or the pthreads library distributed with the OS can be used.
For OpenMP this implies that the highest version of the standard is limited to version 4.5.
 
### Build with appropriate platform specific hardening options
Another important requirement is that the programs built may safely be used in
in system administrative tasks, may be run with a privileged effective UID. Appropriate
hardening options, applicable to the target platform and the tooling available, must be
configured for all executables and libraries built.

The primary reference for deciding hardening options specification:
https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
(last visited: 20241213).

Maximum direct control over such platform-specific features has priority over portability,
and portability to non GNU/Linux platforms is not in scope at all. Direct control is
achieved by specifying toolchain specificities and options for the tooling directly in
a `CMakePresets.json` file. There is a division of labour between this file and the
`CMakeLists.txt` file:
- `CMakeLists.txt` restricts itself as much as possible to specifying *what* is to be build
  from which particular sources. This includes the specification of any dependencies on
  external libraries and their fetching and enabling.
- `CMakePresets.json` specifies *how* to build, with what "generator", compiler, linker,
  and which options to use (for optimization, for hardening, for verbosity of the configure
  and build processes themselves, etc.).

For more details see: `CONFIGURE+BUILD.README.md`

## IDE support
Of course configuring and building out of source release in "debug" and "release" mode
with the configuration files has been tested from the command line (see also :
`config+build.sh`).  But sometimes support for an IDE is convenient. The current files have
been tested with the QT creator IDE (version 16.x) which has very mature support for CMake.
Also Microsoft's Visual Studio Code (VScode) has good support for CMake using a
`CMakeLists.txt` file in conjunction with a `CMakePresets.json` file.

## Focus on compile-time computing
A lot of the core information pertaining to downtime analysis is stable for well-defined
periods of time: which nodes belong to the production environment, the node type of each
node, and the cost to be attributed to a node of a particular type being non-available, etc.
From the software developers perspective, these data are all known at compiletime. It is
a goal, or at least a focus, of the project to exploit this situation by making use of
compiletime computing features of modern C++: `constexpr`, `consteval`, and compiletime
testing with `static_assert`.

### Compiletime regular expressions (CTRE)
With the amount of detail being known at compiletime, regular expressions, used for
parsing nodename expressions, are also known at compile time. There is no need at all
for runtime input of the expression and so also no need at all for runtime compiling
reqular expressions into some processing device. However, the C++ standard library
currently does not include compiletime compilation of regular expressions. The
external library that does, and that is used by this project is the "header-only"
CTRE library developed by Hana Dusíková.

The downloading and configuration of the libary, so that it can be used by a simple
`#include <ctre.hpp>` preprocessor directive in the project's source files, is
arranged in the `CMakeLists.txt` configuration file.


For more details see: `Using CTRE.md`
