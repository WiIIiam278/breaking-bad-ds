<h1 align="center">
    <img alt="Cover artwork by GardenOwl" src="cover-artwork.png" width="185px" />
    <br/>
    Breaking Bad DS
</h1>

Breaking Bad, as a 3D Nintendo DS game.

## Building
BlocksDS, NFLib and Nitro-Engine must be installed. Docker reccommended for building on Windows as a result. Then, just run the following to build a `.nds` ROM file:

```bash
make
```

## Playing
This relies heavily on features of the Nintendo DS 3D engine, and won't run great on emulators with low accuracy. melonDS is reccommended as a result&mdash;though you'll need to go into the Emulator Config -> DLDI -> Enable DLDI, since this uses the homebrew nitrofile system. Or, pop the ROM on any flashcart and enjoy.

## License
Licensed under Apache-2.0, except otherwise stated. For more information, see [LICENSE].

For attribution information on some of the resources used, please see [ATTRIBUTION.md].