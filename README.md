> Under active development. iON is being built in a layered strategy, where I am building it in functional layers and then adding features in each layer. Each layer will be released as a seperate package.

# iON
iON is a standalone global register allocator targeting AArch64. iON takes in an intermediate representation (IR) program with virtual registers (VRs) and produces an allocated IR -- the same representation it recieved but with every virtual register replaced with a physical register.

## Building (macOS / Linux)

**Prerequisites:** CMake 3.20+, a C++20-capable compiler (Clang or GCC), and Git (for fetching GoogleTest).

```bash
# Configure (Debug by default)
cmake -S . -B build

# Build
cmake --build build

# Run tests
cd build && ctest --output-on-failure
```

To build in Release mode:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

To skip building tests:

```bash
cmake -S . -B build -DION_BUILD_TESTS=OFF
cmake --build build
```
