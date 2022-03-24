# JHC
JHC(JiangXueqiao's helper classes, who's JiangXueqiao? I have no idea), a header-only C++ common library that don't need install any third party library.

# How to use
Only need include `jhc/include` folder in your project, then include then specific header file at your code, such as:
	
```c++
#include "jhc/json.hpp"
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
