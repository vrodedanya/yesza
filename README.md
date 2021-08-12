# Yesza library
## Description
This library allows you to process function equation (not ready now:)) or solve problems in c++ standard strings by reversed polish notation
## Install
Copy header to your project
## Example
```c++
auto func = yesza::make_equation("50+60*sin(45+456)*(250-30)");
std::cout << func() << std::endl;
```
