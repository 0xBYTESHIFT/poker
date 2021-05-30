# Pre-build

[![build](https://github.com/0xBYTESHIFT/poker/actions/workflows/docker-build.yml/badge.svg)](https://github.com/0xBYTESHIFT/poker/actions/workflows/docker-build.yml)

```
pip3 install conan cmake
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.cppstd=17 default
```

# Build
```
mkdir build && cd build
export CXX=clang++-11 && export CC=clang-11 #or some other compiler
conan install .. -s compiler=clang -s compiler.version=11 --build=missing 
cmake .. -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX
cmake --build . --parallel
```

# TODO

- [ ] Account
    - [x] Register
    - [x] Login
    - [x] Unlogin
    - [x] Unregister
    - [ ] Donate
- [ ] Rooms
    - [x] Make room
        - [x] Make passworded rooms
        - [x] Make hidden rooms
        - [ ] Make named rooms
    - [x] List rooms
    - [x] Enter room
    - [x] Leave room
- [ ] Poker
    - [ ] Start game
    - [ ] Leave game
    - [ ] Request cards
    - [ ] Make a bet
    - [ ] Split bank
    - [ ] Rang
    - [ ] Matchmaking
- [ ] Social
    - [ ] Adding friend
    - [ ] Removing friend
    - [ ] Room invites
