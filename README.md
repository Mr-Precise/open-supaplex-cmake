# OpenSupaplex

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)  

[Supaplex](https://en.wikipedia.org/wiki/Supaplex) is a game made in the early nineties.
* This is a fork [sergiou87/open-supaplex repo](https://github.com/sergiou87/open-supaplex)

![image](https://github.com/sergiou87/open-supaplex/raw/master/switch/resources/screenshots/OpenSupaplex-5.png)

Since then the source code has been lost. Some enthusiasts have disassembled the game,
fixed many bugs, and added many features. Today the game and the so-called speed fix can
be downloaded free from [here](http://www.elmerproductions.com/sp/dlinst.html). The source code for the speed fix is likewise, unavailable.

**OpenSupaplex** is a 1:1 reimplementation of the original game in C and SDL, with just a few changes to make it easy to use with game controllers, and also easy to port to any platform.

## Getting Started

If you want to build the original game from its disassembled source code, please refer to
the instructions in [cilliemalan's repo](https://github.com/cilliemalan/supaplex).

Other than that, **OpenSupaplex** can be built for any of the following platforms:
- macOS
- Windows
- Linux  
- Other from original code: [sergiou87/open-supaplex repo](https://github.com/sergiou87/open-supaplex)

More platforms (like Android, iOS and tvOS) to come Soon™.

### Prerequisites
Depending on the platform, you need to install a different toolchain to build the game for that platform.
You will also need to install the libraries needed to make it work. Those are basically SDL and SDL_mixer,
and their dependencies. All ports use SDL 2, except for PSP, Nintendo DS, Nintendo 3DS, Nintendo Wii and
RISC OS, which use SDL 1.2.


### Building
Once the development environment is set up, building the game is usually as easy as:

Install CMake for your OS.  
Install dev packages SDL2, SDL2_Mixer
 
Linux build: 
``` 
mkdir build && cd build
cmake -G "Unix Makefiles" ..
make
```  
MinGW32 or MXE CrossCompiling: 
```
mkdir build && cd build
cmake -G "Unix Makefiles" .. -DCMAKE_TOOLCHAIN_FILE=Your-CrossCompiling-Toolchain-file.cmake
make
```

On macOS, not yet tested...

## Contributing

If you like this project, there are several ways you can contribute to it:
- **Reporting bugs:** just file an issue in this repo, detailing the bug and how to reproduce it as much as you can.
It's even better if you can record a demo of the bug using the demo feature of the game and upload the file.
- **Requesting features or ports**: again, filing an issue in this repo is the way to go, detailing what you need as much as you can.
- **Submitting Pull Requests:** you can also contribute with your own code. Just, please, be mindful of the code style and project structure used
already.

### Contributing with new features

New features are more than welcome. However, the main goal of **OpenSupaplex** is having a 1:1 clone of the
original Supaplex game, in every possible aspect, and only making exceptions wherever is reasonable to do so.

**New features must NOT change how the game works or looks in a way that CANNOT be avoided.** For those kind of
features, adding some kind of setting so users can opt-in is the way to go.

### Contributing with ports to new platforms

The whole reason for **OpenSupaplex** to exist is to make the experience of the original game available natively to as many platforms
as possible. If you want to bring **OpenSupaplex** to a new platform, please go for it!

But please be mindful of how ports are structured already in the repo:
- Create a subfolder in the root of the repo with the (lowercase) name that represents the new platform.
That folder should contain everything needed to build and bundle the game for distribution.
- If needed, inside the `src` folder you can create also a folder with the (lowercase) name of the platform
and add specific implementation files there.
- If you can, avoid plaguing the code with `#if` statements for the new platform.
- If you find a bug that only happens in the new platform, and presumably is a bug in either SDL,
SDL_mixer or any of their dependencies, please, fix the root of the problem and don't write workarounds
in the game's codebase. Not only you won't pollute **OpenSupaplex** with hacks that belong elsewhere,
but you will also be contributing and helping people using SDL and SDL_mixer in that platform.
I [have](https://github.com/libsdl-org/SDL_mixer/commit/fb3af0e06e2d27e699ac6102ce5917ef91622e1f) [done](https://github.com/devkitPro/SDL-mirror/pull/59) [that](https://github.com/ps3dev/PSL1GHT/pull/71) [many](https://github.com/ps3dev/ps3libraries/pull/37) [many](https://github.com/rsn8887/SDL-Vita/pull/8) [times](https://github.com/devkitPro/SDL-mirror/pull/57) while working on the different ports, and it's a great experience.
- It'd be very helpful if you also got some CI scripts to allow me building the new port from the GitHub Actions.

### Contributing with feedback

I'm also very open to suggestions about how to make things better. **Constructive** feedback on my code, how to structure the project
to make it easier to handle all the ports… just file an issue and we can discuss it there.

### Hint for packagers for Linux distros

By default OpenSupaplex on Linux reads and writes files in current working directory. To make it more package-friendly, provide the following in CFLAGS: `-DFILE_FHS_XDG_DIRS -DFILE_DATA_PATH=/usr/share/OpenSupaplex`

## License
As expressed in [cilliemalan's repo](https://github.com/cilliemalan/supaplex), the
disassembled assembly source code of the game is released under the MIT license.

This repository also includes the resources from the original game, now distributed as
freeware, that belong to their original authors.

On the other hand, I release the reimplementation in C under the GPLv3 license.

If you have a problem with this please don't hesitate to contact me.
