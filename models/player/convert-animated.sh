cd ../../../../nitro-engine/tools/md5_to_dsma &&
python md5_to_dsma.py --model ../../../projects/breaking-bad-ds/models/player/player.md5mesh \
               --name player \
               --output ../../../projects/breaking-bad-ds/nitrofiles/model \
               --texture 128 128 \
               --anim ../../../projects/breaking-bad-ds/models/player/walk.md5anim ../../../projects/breaking-bad-ds/models/player/idle.md5anim &&
cd ../img2ds &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/player/walter.png \
                --name walter \
                --output ../../../projects/breaking-bad-ds/nitrofiles/model \
                --format A1RGB5 &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/player/jessie.png \
                --name jessie \
                --output ../../../projects/breaking-bad-ds/nitrofiles/model \
                --format A1RGB5 &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/player/yeppers.png \
                --name yeppers \
                --output ../../../projects/breaking-bad-ds/nitrofiles/model \
                --format A1RGB5