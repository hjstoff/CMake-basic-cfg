# Using compile time regular expressions

## Abstract
Compile time regular expressions (CTRE), developed by Hana Dusíková already around 2017,
are a great improvement for over the C++ standard library regular expression support.
Their runtime performance gain is easily understood and also well-documented. Mainly
for two reasons however, I found them not so very easy to use:

1. Syntax details are different for compilers with C++17 support and compilers with
   (a sufficient level of) C++20 support. The C++20 is definitely more readable, much
   less of an awkward workaround.
2. The how-to-use aspects of the library are under-documented. The frequent use of
   the `auto` keyword, both in the `ctre.hpp` file itself and in the few usage
   examples given in the documentation, hides what the actual return types are, and
   consequently what member functions these have.
   In particular, the details of how to work with the captures of a successful match
   only became clear to me after studying the details of the `ctre.hpp` header file
   and a considerable amount of experimenting.

The following is a use-oriented summary of CTRE. It focusses on use with compilers 
that have a sufficient level of C++20 support. I used GNU's g++, version 11.3 and higher,
which defaults to compiling C++ sources according to the C++17 language standard, but 
seems to have no problems with C++20 CTRE syntax when compilation is done with the
`-std=c++20` option.

## Getting the library

The CTRE library is header-only. It is available as a single header implementation:
[https://github.com/hanickadot/compile-time-regular-expressions/tree/main/single-header/ctre.hpp]

It can be downloaded in a directory among your sources files where it is needed, and then
treated as a local header, including it in files where it is needed, like so:
```cpp
#include "ctre.hpp"
```
I prefer to treat it as an external dependency and have control over the particular 
version that is used, by including the following in the `CMakeLists.txt` file of a project
that uses the library:

```CMake

include(FetchContent)
FetchContent_Declare(
        ctre
        GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
        GIT_TAG v3.10.0
)
FetchContent_MakeAvailable(ctre)
link_libraries(ctre::ctre)
```
Even though all of ctre is header-only, and linking in the proper sense is not applicable,
link_libraries is required, as it also takes care of the compile flags. To have a working
`include <ctre>` include statement in the sources, the compiler must be invoked with
`-I<somedir>`, where <somedir> denotes the path where CMake put the stuff it fetched from github.

## Specifying the regular expression
*The regular expression to be matched is specified as a template argument* that is a
conventional fixed string. For non-trivial regular expressions, "raw" string syntax
is preferred to increase readability, like so:

```cpp
int main()
{
	static constexpr const char expr[] =
   	R"(^(([fght]cn)|(srv))(([1-9][0-9]*)|(\[[1-9][0-9]*(-[1-9][0-9]*)?(,[1-9][0-9]*(-[1-9][0-9]*)?)*)\])$)";
	//0 12        2 3   3145           5 6             7            7 8            9            9 8 6  4 0    
	static constexpr auto pattern = ctre::match<expr>;

	. . . 
}
``` 

In the above code snippet, `pattern` is the object holding the result of the compilation of
`expr`. That is, it is a deterministic finite automaton (DFA) equivalent structure, but
rather than a traditional table-driven DFA it has states encoded as template instantiations
and state transitions encoded as functions. Several optimization have been applied.
The DFA's "state table" is essentially baked into the compiled binary as optimized machine
code rather. This gives you DFA-like deterministic behavior with very good performance
characteristics.

Because the `auto` keyword is used, the type of `pattern` remains implicit. It is not of
much practical value. The most important thing to know to use it productively is that it has
a has a heavily overloaded function call operator - `()` - that can be used with several
types of arguments specifying *what* to match.

The object returned by a `ctre::match()` call is an object from the `ctre::regex_results`
template family. The specifically instantiated type depends, among other things, on the number
of captures specified in the regular expression it was created from. To use the library
effectively, the functionality offered by the `ctre::regex_results` template must be
adequadely documented - which we will do further below.


## Matching with `ctre::match` instances with arguments to its function call operator.

A try to match can have several acceptable arguments that denote a "string" in the generic
sense of a contiguous set of elements of type char. It is *not* required that such a
string-like object is nullbyte terminated. The following modes of usage are all possible:

```cpp
	auto result = ctre::match<"pattern">(string_view);
	auto result = ctre::match<"pattern">(std::string);
	auto result = ctre::match<"pattern">(const char *);
	auto result = ctre::match<"pattern">(const char* str, size_t length);
	auto result = ctre::match<"pattern">(first_iterator, last_iterator);
```

In case a `(const char *)` argument is passed, without a specified length, it is asserted that
the string length is determined by teh the first nullbyte encountered.

## The return value of a `ctre::match()` attempt

The return value of a `ctre::match()` attempt is an instance of a `ctre::regex_results`
template. If the match attempt was not succesful, the return value, used in a boolean
context, is implicitly converted to the boolean value `false`.  If there is a match,
the return value in a boolean context is implicitly converted to `true`.
The syntax for using the submatches is sighly awkward, as this must be done with
a template `get` function where the submatch to get is a template parameter:
0 gives the complete match, 1 is the first submatch, 2 is the second submatch,
and so on, like so:

	`std::string_view submatch1 = regex_matchresult.get<1>();`
	`std::string_view submatch4 = regex_matchresult.get<4>();`

Note that return values are stored in string_view objects.

```cpp
static constexpr const char expr[] = R"(^(([fght]cn)|(srv))([1-9][0-9]{0,3})$)";
/*                                       12        2 3   314               4  
 * Submatches of expr:
 * 1 ::= non-numeric prefix,
 * 2 ::= "fcn", "gcn", "hcn", or "tcn"
 * 3 ::= "srv"
 * 4 ::= numeric suffix, inclusive range: [1-9999].
 */
static constexpr auto pattern = ctre::match<expr>; // pattern is the compiled expr.

constexpr bool f(std::string_view sv)
{
        if (auto result = pattern(sv)) {	//  Call operator with string_view argument
                std::string_view prefix = result.get<1>();
		(...)
                std::string_view suffix = result.get<4>();
		(...)
		return true;
        }
        return false;
}
```
