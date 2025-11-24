Qt Creator 16.x has excellent support for CMake projects with CMakePresets.json
 files, and you can import your existing project with minimal or no changes
to your configuration.

Here's how to do it:

- Open Qt Creator and choose "Open Project";
- Navigate to your project directory and select either the `CMakeLists.txt`
  file, or the `CMakePresets.json` file directly.

Qt Creator will automatically detect and parse your CMakePresets.json file.

What Qt Creator Does Automatically

- Preset Detection: Qt Creator reads your CMakePresets.json and presents
  the available configure presets in the project configuration dialog

- Build Integration: It maps your configure presets to build configurations

- Toolchain Setup: If your presets specify toolchains, Qt Creator will attempt
  to use them

- Environment Variables: Variables defined in your presets are automatically
  applied


Configuration in Qt Creator
After import, you can:

Select Presets: Go to Projects mode (Ctrl+5) and choose which CMake preset
to use for each build configuration

Build Presets: Qt Creator also supports build presets if you have them
defined in your CMakePresets.json

Test Presets: Test presets are supported for running CTest

Potential Adjustments (Usually Minimal)
You might need to:

Kit Configuration: Ensure Qt Creator can find your compiler toolchain if
it's not in standard locations
Qt Integration: If your project uses Qt but doesn't explicitly find it
in CMake, you might need to set Qt paths in the project settings

Best Practices

Keep your existing CMakePresets.json structure - Qt Creator works best with
standard CMake preset conventions

Use the "Configure Project" button in Qt Creator to let it automatically set
up build directories based on your presets

Check the "Issues" panel if Qt Creator has trouble finding compilers or
dependencies

Qt Creator's CMake preset support is quite mature, so in most cases, your
existing configuration should work without any modifications to your
CMakeLists.txt or CMakePresets.json files.

In the CMakePreset.json file I use Cachevariables "CMAKE_C_COMPILER":
"/usr/bin/gcc" and "CMAKE_CXX_COMPILER": "/usr/bin/g++" to specify the
toolchain.

Is that the best way or would you recommend another approach?

Your current approach using cache variables in CMakePresets.json is perfectly
valid and widely used, but there are some alternative approaches that might
be more robust depending on your needs:
Your Current Approach (Good)
json{
  "cacheVariables": {
    "CMAKE_C_COMPILER": "/usr/bin/gcc",
    "CMAKE_CXX_COMPILER": "/usr/bin/g++"
  }
}
This works well and is straightforward, especially for simple setups.
Alternative Approaches
1. Using Toolchain Files (Most Robust)
Create a separate toolchain file and reference it:
toolchain-gcc.cmake:
cmakeset(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_SYSTEM_NAME Linux)  # if cross-compiling
CMakePresets.json:
json{
  "cacheVariables": {
    "CMAKE_TOOLCHAIN_FILE": "${sourceDir}/cmake/toolchain-gcc.cmake"
  }
}
2. Using Environment Variables (More Flexible)
json{
  "environment": {
    "CC": "/usr/bin/gcc",
    "CXX": "/usr/bin/g++"
  }
}
3. Hybrid Approach (Recommended for Cross-Platform)
json{
  "environment": {
    "CC": "/usr/bin/gcc",
    "CXX": "/usr/bin/g++"
  },
  "cacheVariables": {
    "CMAKE_BUILD_TYPE": "Release"
  }
}
Recommendations

For your Qt Creator use case, I'd recommend sticking with your current
approach because:

Qt Creator Integration: Qt Creator handles cache variables very well and
shows them clearly in the project configuration UI

Explicit: It's clear and explicit what compilers are being used
Per-Preset Control: Easy to have different presets with different compilers

Consider the toolchain file approach if:

- You have complex cross-compilation needs
- You need to set additional compiler flags, linker settings, or
  system-specific variables
- You want to share toolchain configuration across multiple projects

Consider environment variables if:

You want more flexibility to override compilers at build time
You're working in environments where compiler paths might vary

For Qt Creator with a straightforward Linux development setup, your current
cache variable approach is actually quite good and requires no changes.

