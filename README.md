<h1 align="center">
    <img alt="Cover artwork by GardenOwl" src="cover-artwork.png" width="185px" />
    <br/>
    Breaking Bad DS
</h1>

*Breaking Bad*, as a Nintendo DS game, complete with 3D graphics and models. Master the art of the cook on your Nintendo DS system and prepare the perfect batch&mdash;lest you meet the wrong end of Gus.

## Playing
For the absolute best experience, pop the ROM on a flashcart or a DSi/3DS with TwiLight Menu++ and enjoy!

But if you're not as bothered as Walter is about doing things with the proper gear, [melonDS](https://github.com/melonDS-emu/melonDS/releases/latest) is the recommended emulator for play due to its accuracy and performance. Alternatively, the latest release of [DeSmuME](https://github.com/TASEmulators/desmume/releases/latest) is slightly easier to setup and supports [emulating rumble](#ds-rumble-pak-support).

To run this on melonDS, you'll need to go into the Emulator Config -> DLDI -> Enable DLDI, since this uses the homebrew nitrofile system.  I reccommend the Software video emulator for best results to make the models pop, as the OpenGL backend doesn't quite support the DS's hardware-accellerated outlines just yet.

This won't work on the Wii U Virtual Console emulator. I'm sorry to ruin your dreams. If you run this on a cool setup, take a picture and [email me](mailto:will27528+brbads@gmail.com).

### DS Rumble Pak support
This game supports the [Nintendo DS Rumble Pak](https://en.wikipedia.org/wiki/Rumble_Pak#Nintendo_DS)! DeSMuME supports emulating this &mdash; or you can use a physical SLOT-2 DS Rumble Pak accessory on an original DS/DS Lite.

## Building
### General instructions
devKitARM, NFLib and Nitro-Engine must be installed. Then, just run `make` following to build a `.nds` ROM file.

### Asset preparation
There's scripts for converting assets and moving them into the correct nitrofiles directories. Run `bash ./convert.sh` inside `graphics/sprite` for instance to automatically prepare all the sprite images. If you've updated the graphics, delete the `build/` directory before building.

## License
The source code for this project is licensed under Apache-2.0, except otherwise stated. For more information, see [LICENSE](https://github.com/WiIIiam278/breaking-bad-ds/blob/main/LICENSE).

For attribution information on the various resources I used, please see [ATTRIBUTION.md](https://github.com/WiIIiam278/breaking-bad-ds/blob/main/ATTRIBUTION.md).