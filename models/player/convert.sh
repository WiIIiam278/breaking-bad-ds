# Sadly we're only exporting the static player model as the dsma library seems to have issues with the armature when loading the converted version in-game :(
cd ../../../../nitro-engine/tools/obj2dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/new-player/player.obj \
                --texture 128 128 \
                --scale 100.0 \
                --output ../../../projects/breaking-bad-ds/nitrofiles/model/player.dl &&
cd ../img2ds &&
convert ../../../projects/breaking-bad-ds/models/new-player/walter.png -flip ../../../projects/breaking-bad-ds/models/new-player/walter.png &&
convert ../../../projects/breaking-bad-ds/models/new-player/jessie.png -flip ../../../projects/breaking-bad-ds/models/new-player/jessie.png &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/new-player/walter.png \
                --name walter \
                --output ../../../projects/breaking-bad-ds/nitrofiles/model \
                --format A1RGB5 &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/new-player/jessie.png \
                --name jessie \
                --output ../../../projects/breaking-bad-ds/nitrofiles/model \
                --format A1RGB5 &&
convert ../../../projects/breaking-bad-ds/models/new-player/walter.png -flip ../../../projects/breaking-bad-ds/models/new-player/walter.png &&
convert ../../../projects/breaking-bad-ds/models/new-player/jessie.png -flip ../../../projects/breaking-bad-ds/models/new-player/jessie.png