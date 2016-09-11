# Ludo
General purpose game engine.

# Build State
<a href="https://scan.coverity.com/projects/tleonarduk-ludo">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/9006/badge.svg"/>
</a>

## Preface
The aim of this project is primarily to produce a rapid-prototyping, generic, cross platform, game engine, for use in internal projects. It is being made open source in case it comes in useful for anyone else.

## Building from source
Ludo uses premake for its build system. The binaries for premake are included in the depot. To generate solution configurations for the project go into the Build folder and run the appropriate script file for your platform. This will generate solution files in the Build/ProjectFiles directory.

Solution files should always be considered temporary, and should be regenerated whenever the user syncs to latest.

## Building third-party libraries from source
The game includes various third-party libraries. In most cases these are included in the generated premake solution files and will be build along with the engine. However some third party libraries have complex build rules that are difficult to integrate into premake. These ones are distributed in the depot in binary form, rebuilding them needs to be done per-library, check their readme's for details. 

All third party source is contained in Engine/Source/ThirdParty.

Libraries currently requiring manual rebuilds:

+ LibClang

## Debugging Setup
If your using visual studio, make sure to install the natvis files in Tools/VisualStudio to make debugging complex types a bit simpler.

## Project file layout

+ Build
Batch files for generating solution files for different IDE's.
++ Build\Config
Premake configuration files detailing how solution files are generated.
+ Engine
++ Engine\Binaries
Contains output from building. Should be considered temporary, and should never be checked into source control.
++ Engine\Build
Contains configuration data determining how premake should build the engine.
++ Engine\Content
Contains all content used directly by the engine. 
++ Engine\Source
Contains source code for the engine.
+++ Engine\Source\Builder
Source code for the builder binary, which is responsible for helping build content, such as generation of reflection data.
+++ Engine\Source\Core
Source code for the core static library, contains boilerplate code that is not game or engine relevant. Such as containers/math/etc.
+++ Engine\Source\Editor
Source code for the game editor binary.
+++ Engine\Source\Framework
Source code for the game-framework static library, contains code shared between the editor and runner. Such as scene-graphs/actors/cameras/etc.
+++ Engine\Source\Platforms
All platform specific static libraries is contained in individual folders here.
+++ Engine\Source\Runner
Source code for the runner binary. This is responsible for loading a game dll and executing it as a game.
+++ Engine\Source\ThirdParty
All third-party libraries and source code is stored here.
++ Engine\SourceArt
Contains source art assets for before importation into the engine.
+ Projects
Contains folders for all projects that use the game engine. They compile to dll's that are loaded and executed by the editor/runner. The folder format is the same structure as the Engine folder (eg. source/sourceart/content/build/etc folders). Whenver you create a new project folder, run premake again to add it to the main engine solution.
+ Tools
Contains any tools used by the engine, or helpful to the developer.

## Reflection
Ludo uses a custom reflection system, source for which can be viewed in Engine/Source/Core/Reflection. In general this should be transparent to the developer. However its worth pointing out that reflection data generation involves an extra pre-compile step that runs the code through libclang to extact meta-data. Which can make build times longer if a lot of header files are modified. Meta-data extraction is done incrementally so if you change your clock or move temporary files between computers, make sure to force a full rebuild.
