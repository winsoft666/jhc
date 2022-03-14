# JHC
JHC(JiangXueqiao's helper classes), a header-only C++ common library that don't need install any third party library.

# Illustrate
`json.hpp` is provided by [nlohmann/json](https://github.com/nlohmann/json), I renamed namespace from `nlohmann` to `akl`.

`filesystem.hpp` is provided by [gulrak/filesystem](https://github.com/gulrak/filesystem), I renamed namespace from `ghc` to `akl`.

`base64.hpp` is provided by [ReneNyffenegger/cpp-base64](https://github.com/ReneNyffenegger/cpp-base64) and adapted by me.



# How to use
Only need include `jhc/include` folder in you project, then include header file at your code, such as:
	
```c++
#include "jhc/json.hpp"
```


# Build Test

```bash
cd test
mkdir build
cd build
cmake ..
make
./test
```
