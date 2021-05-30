# Pre-build

[![build](https://github.com/0xBYTESHIFT/poker/actions/workflows/docker-build.yml/badge.svg)](https://github.com/0xBYTESHIFT/poker/actions/workflows/docker-build.yml)

```
pip3 install conan
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan profile new default --detect
conan profile update settings.compiler.libcxx=libstdc++11 default
conan profile update settings.cppstd=17 default
```

# Build
```
mkdir build && cd build
conan install ../recipies/connector --build=missing
conan create ../recipies/connector local/stable

conan install .. --build=missing
cmake .. 
cmake --build . --config Release
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
    - [ ] Matchmaking
