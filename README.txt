README.txt

This repository contains the basic setup for a CMake C/C++ project on a
GNU/Linux platform, using the gcc / g++ compilers


To use CMakeLists.txt.template:
- Define a real project name (instead of myproject), and possibly adjust also
  the version in the project command.
- Specify at least one executable to be built and the list of source files on
  which it depends.
- Rename the template to CMakeLists.txt

To use CMakePresets.json.template:
- Rename to CMakePresets.json.

N.B.: 
CMakePresets.json, and other files, can in principle be used as is, without
any customization. CMakePresets.json and CMakeLists.txt are specific CMake
files that MUST be thus named. Although there is in priciple no need to
customize the contents of the provided CMakePresets.json the .template suffix
is added to prevent that unpacking these files from a tar file accidentally
overwrites a custom CMakePresets.json file that already exists in the
unpacking directory.

For further reading see the MarkDown documents in directory mddocs
