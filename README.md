<h1 align="center">
    <img alt="Cover artwork by GardenOwl" src="cover-artwork.png" width="185px" />
    <br/>
    Breaking Bad DS
</h1>

*Breaking Bad*, as a 3D Nintendo DS game.

## Building
### General instructions
devKitARM, NFLib and Nitro-Engine must be installed. Then, just run `make` following to build a `.nds` ROM file.

### Asset preparation
There's scripts for converting assets and moving them into the correct nitrofiles directories. Run `bash ./convert.sh` inside `graphics/sprite` for instance to automatically prepare all the sprite images. If you've updated the graphics, delete the `build/` directory before building.

## Playing
This relies heavily on features of the Nintendo DS 3D engine, and won't run great on emulators with low accuracy (DeSMuME won't load, for instance). [melonDS](https://github.com/melonDS-emu/melonDS/) is reccommended as a result&mdash;though you'll need to go into the Emulator Config -> DLDI -> Enable DLDI, since this uses the homebrew nitrofile system. Or, pop the ROM on any flashcart and enjoy.

### DS Rumble Pak support
This game supports the [Nintendo DS Rumble Pak](https://en.wikipedia.org/wiki/Rumble_Pak#Nintendo_DS). Unfortunatlely, since DeSMuME doesn't work and Melon doesn't support the Rumble Pak yet, you'll need to buy a physical SLOT-2 DS Rumble Pak accessory to use this feature.

## License
The source code for this project is licensed under Apache-2.0, except otherwise stated. For more information, see [LICENSE](https://github.com/WiIIiam278/breaking-bad-ds/blob/main/LICENSE).

For attribution information on some of the resources used, please see [ATTRIBUTION.md](https://github.com/WiIIiam278/breaking-bad-ds/blob/main/ATTRIBUTION.md).