# fixed
## Copyright
Public domain, 2025

github.com/SirJonthe

## About
`fixed` is a C++11 implementation of a fixed-point data type with customizable width and precision.

## Design
The library mainly provides the user with a single class for use, `fixed`, which is meant to act as a sort of alternative to floating-point data types.

## Building
No special adjustments need to be made to build `fixed` except enabling C++11 compatibility or above. Simply include the relevant headers in your code and make sure the headers and source files are available in your compiler search paths. Using `g++` as an example, building is no harder than:

```
g++ -std=c++11 code.cpp fixed/fixed.cpp
```

...where `code.cpp` is an example source file containing some user code as well as the entry point for the program.

## Examples
### Creating a fixed-point number
Fundamentally, the user needs to determine what bit width and precision to use for computations. Exactly what should be used completely depends on the use-case. In this example, 32 bits for width is used (i.e. the total number of bits representing the number is 32), with a precision of 15 bits (i.e. out of the 32 available bits, 15 are used for fractions).
```
#include "fixed/fixed.h"

cc0::fixed<32,15> f;
```
Currently, there are four bit widths available to use; 8, 16, 32, and 64. Specifying anything but a supported bit width will result in a compiler error. Note also that there are no safe-guards preventing the precision parameter to exceed the available bits of the underlying data type. The precision should take into account that one bit needs to be used for the sign, and one bit needs to be used for whole numbers, although most cases will call for a more balanced relationship between the width and precision than the extremes.

A fixed-precision number can be set to a value in one of two ways; using integers, and using a custom two-integer representation of a fixed-point number:
```
#include "fixed/fixed.h"

cc0::fixed<32,15> a = 15;
cc0::fixed<32,15> b = cc0::fixed<32,15>(15,5);
```
The two-integer solution is similar to how floating-point numbers can be initialized using constant literals; The first signed integer represents the whole part of the fixed-point number, while the second unsigned integer represents the decimal fractional part of the fixed-point number. Regarding the decimal fractional part - The fraction is properly scaled so that 5 will result in the scale fraction as 50, and 500, etc.

A `fixed` instance is always signed.

### Arithmetic
Arithmetic functions in much the same way as any other data type in C/C++:
```
#include "fixed/fixed.h"

cc0::fixed<32,15> a = 15;
cc0::fixed<32,15> b = cc0::fixed<32,15>(15,5);
cc0::fixed<32,15> c = b + a * b - a + 10;
```
Integers are automatically type upcasted into a `fixed` data type and are supported natively in arithmetic.

Note that division by zero is handled the same way as for integers.

### Comparisons
Comparisons function in much the same way as any other data type in C/C++:
```
#include "fixed/fixed.h"

cc0::fixed<32,15> a = 15;
cc0::fixed<32,15> b = cc0::fixed<32,15>(15,5);

if (a == b) { /**/ }
if (a == 10) { /**/ }
if (15 == b) { /**/ }
if (a > b) { /**/ }
if (a <= b) { /**/ }
```
Integers are automatically type upcasted into a `fixed` data type and are supported natively in comparisons.

## Warnings
`fixed` is a combination of a C/C++ integer and floating-point number insofar as it contains features and limitations of both. `fixed` does not implement representation for not-a-number, positive and negative infinity, or positive and negative zero. Similarly, a division by zero will result in the same behavior as a division by zero using integers. This means that `fixed` is not a one-to-one drop-in replacement for floating-point numbers, but rather, special care needs to be taken when using the data type. `fixed` also suffers from potential overflows and underflows similar to integer data types, but the risk is compounded since the data representation scales the integer part of the number to the higher bits of the underlying bit representation. Because of this, the user needs to take special care to select a proper width and precision for the specific use case, especially when multiplication and division is involved.