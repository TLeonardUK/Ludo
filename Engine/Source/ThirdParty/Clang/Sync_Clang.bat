"C:/Program Files (x86)/Git/bin/git.exe" clone https://github.com/llvm-mirror/llvm.git
cd llvm
"C:/Program Files (x86)/Git/bin/git.exe" pull origin master
cd tools
"C:/Program Files (x86)/Git/bin/git.exe" clone https://github.com/llvm-mirror/clang.git
cd clang
"C:/Program Files (x86)/Git/bin/git.exe" pull origin master
cd ../../..