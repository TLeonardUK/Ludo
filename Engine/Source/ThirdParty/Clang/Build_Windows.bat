set "PATH=C:\Program Files (x86)\MSBuild\14.0\Bin\;C:\Program Files\CMake\bin;%PATH%"

mkdir build.x86_64
mkdir build.x86

cd build.x86_64

cmake ../llvm -G "Visual Studio 14 Win64" -DCMAKE_BUILD_TYPE=Release -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_GO_TESTS=OFF -DLLVM_INCLUDE_DOCS=OFF
msbuild tools/clang/tools/libclang/libclang.vcxproj /p:Configuration=Release;Platform=x64 /m:2 /t:Rebuild

cd ..

cd build.x86

cmake ../llvm -G "Visual Studio 14" -DCMAKE_BUILD_TYPE=Release -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_GO_TESTS=OFF -DLLVM_INCLUDE_DOCS=OFF 
msbuild tools/clang/tools/libclang/libclang.vcxproj /p:Configuration=Release;Platform=Win32 /m:2 /t:Rebuild

cd ..

