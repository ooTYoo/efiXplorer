[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)

**efiXplorer** - IDA plugin for UEFI firmware analysis and reverse engineering automation :octocat:

__Contributors__: 
* Alex Matrosov ([@matrosov](https://github.com/matrosov))
* Andrey Labunets ([@isciurus](https://github.com/isciurus))
* Philip Lebedev ([@p41l](https://github.com/p41l/))
* Yegor Vasilenko ([@yeggor](https://github.com/yeggor/))

__Supported versions of Hex-Rays products:__ everytime we focus on last versions of IDA and Decompiler because trying to use most recent features from new SDK releases. That's mean we tested just on recent versions of Hex-Rays products and not guaranteed stable work on previous generations. 

__Why not IDApython:__ all code developed on C++ because it's more stable way to support complex plugin and get full power of most recent SDK's features.

__Supported Platforms:__ Win, Linux and OSX (x86/x64).

![overview](img/overview.gif)

# Key features

## Identify availible Boot Services automaticaly

Anotate assembly code atomaticaly with availible Boot Services

![bs2](img/bs2.png)

## Identify availible Runtime Services automaticaly

Anotate assembly code atomaticaly with availible Runtime Services

![rt2](img/rt2.png)

## Identify availible EFI Protocols automaticaly

* Build the list of availible EFI Protocols

    ![protocols](img/protocols.png)

## Identify known EFI GUID's

* Build the list of availible EFI GUID's (include protocol name identification)

    ![guids](img/guids.png)

# efiXplorer Architecture

From the beginning of the project, we focus on building extensible architecture to make our  life easier to support the current version and adding new features :rocket:

![arch](img/arch.png)

# Build instruction

We try to make the build process for different platforms very simple, just use the build script to automate this process🐍

## Build script

```
Usage: build.py [OPTIONS] IDASDK_DIR

Options:
  -c, --copy TEXT  path to IDA plugins directory
  --help           Show this message and exit.
```

example of build process:

```bash
./build.py <IDASDK75_DIR>
```

# Installation

Copy binaries from `bin/<platform>/` and `guids` directory to `<IDA_DIR>/plugins`.

# References 

* https://github.com/yeggor/UEFI_RETool 
* https://github.com/gdbinit/EFISwissKnife 
* https://github.com/snare/ida-efiutils
* https://github.com/al3xtjames/ghidra-firmware-utils
