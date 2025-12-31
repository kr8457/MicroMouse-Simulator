### Trailing Return Types
```cpp
auto fuction_name(param 1, param 2) -> type {}
```
### Reasons:
#### 1. Complex Template Return Types 
When the return type depends on the types of the parameters. It puts the return types after the parameters so the compiler can figure out the return type easily (compilers read from left to right)

```cpp
// THIS WILL FAIL: The compiler doesn't know what 'a' and 'b' are yet.
template <typename T, typename U>
decltype(a + b) add(T a, U b) { 
    return a + b; 
}

// THIS WORKS: By moving the type to the end, 'a' and 'b' are already declared.
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}
```

#### 2. Readability and "Top-Down" Parsing
Many modern C++ developers prefer trailing return types for visual alignment. It places all function names at the same starting column, making a header file much easier to scan.
```cpp
// Harder to scan (function names are zig-zagged)
int getHealth();
std::vector<std::string> getNameList();
long long int getTimestamp();

// Easier to scan (names are perfectly aligned)
auto getHealth()   -> int;
auto getNameList() -> std::vector<std::string>;
auto getTimestamp()-> long long int;
```