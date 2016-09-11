LLVM and clang are exceptionally complicated, we don't attempt to build them through 
our own project system. Instead there are various bat and shell files in the clang 
directory which will build it through its native build tool (CMake).

To get latest and build just run:

Sync_Clang.bat/sh

followed by:

Build_Platform.bat/sh