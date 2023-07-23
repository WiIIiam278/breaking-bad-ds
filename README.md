<h1 align="center">
    <img alt="Cover artwork by GardenOwl" src="cover-artwork.png" width="185px" />
    <br/>
    Breaking Bad DS
</h1>

*Breaking Bad*, as a 3D Nintendo DS game.

## Building
### Windows instructions
You can use the Powershell script `build.ps1` to automate the build for Windows. This requires Docker and a certain directory structure setup, though. Here's how to set that up:

* Install [Docker Engine](https://docs.docker.com/engine/) for Windows
* Create a directory somewhere in your machine (preferebly as part of a path without spaces). Name it `Nintendo-DS-Development`
* Create a subdirectory, `projects`. Run `cd projects`, then `git clone https://github.com/WiIIiam278/breaking-bad-ds.git ./breaking-bad-ds`
* Back in the `Nintendo-DS-Development` directory, git clone NightFoxLib and Nitro-Engine into `nds_nflib` and `nitro-engine`
  - `git clone https://github.com/knightfox75/nds_nflib.git ./nds_nflib`
  - `git clone https://github.com/AntonioND/nitro-engine.git ./nitro-engine`
* Your directory structure should now resemble this:
  - `Nintendo-DS-Development/`
    - `projects/`
      - `breaking-bad-ds/`
        - `build.ps1`
        - `(...)`
    - `nds_nflib/`
      - `(...)`
    - `nitro-engine/`
      - `(...)`
* Ensure docker engine is open and that the engline has initialized. Then, open Powershell and run `./build.ps1`. 
* `breaking-bad-ds.nds` will be output.

VSCode is reccommended for development. Please note the VSCode scripts currently use hard-coded paths for my machine; you'll want to edit these to reference your install directory.

### General instructions
BlocksDS, NFLib and Nitro-Engine must be installed. Then, just run `make` following to build a `.nds` ROM file.

### Asset preparation
There's scripts for converting assets and moving them into the correct nitrofiles directories. Run `bash ./convert.sh` inside `graphics/sprite` for instance to automatically prepare all the sprite images. If you've updated the graphics, delete the `build/` directory before building.

## Playing
This relies heavily on features of the Nintendo DS 3D engine, and won't run great on emulators with low accuracy (DeSMuME won't load, for instance). [melonDS](https://github.com/melonDS-emu/melonDS/) is reccommended as a result&mdash;though you'll need to go into the Emulator Config -> DLDI -> Enable DLDI, since this uses the homebrew nitrofile system. Or, pop the ROM on any flashcart and enjoy.

### DS Rumble Pak support
This game supports the [Nintendo DS Rumble Pak](https://en.wikipedia.org/wiki/Rumble_Pak#Nintendo_DS). Unfortunatlely, since DeSMuME doesn't work and Melon doesn't support the Rumble Pak yet, you'll need to buy a physical SLOT-2 DS Rumble Pak accessory to use this feature.

## License
The source code for this project is licensed under Apache-2.0, except otherwise stated. For more information, see [LICENSE](https://github.com/WiIIiam278/breaking-bad-ds/blob/main/LICENSE).

For attribution information on some of the resources used, please see [ATTRIBUTION.md](https://github.com/WiIIiam278/breaking-bad-ds/blob/main/ATTRIBUTION.md).