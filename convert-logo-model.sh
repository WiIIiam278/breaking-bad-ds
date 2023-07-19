cd ../../nitro-engine/tools/obj2dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/logo/logo.obj --texture 128 128 --output ../../../projects/breaking-bad-ds/nitrofiles/model/logo.dl &&
cd ../img2ds &&
# use imagemagick to flip logo.png ONLY VERTICALLY
convert ../../../projects/breaking-bad-ds/models/logo/logo.png -flip ../../../projects/breaking-bad-ds/models/logo/logo.png &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/logo/logo.png --name logo --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model &&
#  flip logo.png back
convert ../../../projects/breaking-bad-ds/models/logo/logo.png -flip ../../../projects/breaking-bad-ds/models/logo/logo.png 