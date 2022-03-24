# JHC
JHC(JiangXueqiao's helper classes, who's JiangXueqiao? I have no idea), a header-only C++ common library that don't need install any third party library.

# How to use
Clone repository and submodules:

```bash
git clone https://github.com/winsoft666/jhc.git
cd jhc
git submodule update --init --recursive
```

and include `jhc/include` directory to your project, then `#include` any jhc header file or `jhc_all.hpp`:

```c++
#include "jhc/file.hpp"
// or include all header files at once
#include "jhc_all.hpp"
```

# Build Test

```bash
git clone https://github.com/winsoft666/jhc.git
cd jhc
git submodule update --init --recursive

cd test
mkdir build
cd build
cmake ..
make
./test
```
