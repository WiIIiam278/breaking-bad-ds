cd ../../nitro-engine/tools/obj2dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/logo/logo.obj --texture 128 128 --output ../../../projects/breaking-bad-ds/nitrofiles/model/logo.dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/logo/logo_text.obj --texture 128 128 --output ../../../projects/breaking-bad-ds/nitrofiles/model/logo_text.dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/logo/backdrop.obj --texture 128 128 --output ../../../projects/breaking-bad-ds/nitrofiles/model/backdrop.dl &&
cd ../img2ds &&
# use imagemagick to flip logo.png and logo_text.png ONLY VERTICALLY
convert ../../../projects/breaking-bad-ds/models/logo/logo.png -flip ../../../projects/breaking-bad-ds/models/logo/logo.png &&
convert ../../../projects/breaking-bad-ds/models/logo/logo_text.png -flip ../../../projects/breaking-bad-ds/models/logo/logo_text.png &&
convert ../../../projects/breaking-bad-ds/models/logo/backdrop.png -flip ../../../projects/breaking-bad-ds/models/logo/backdrop.png &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/logo/logo.png --name logo --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/logo/logo_text.png --name logo_text --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/logo/backdrop.png --name backdrop --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model &&
#  flip logo.png and logo_text.png back
convert ../../../projects/breaking-bad-ds/models/logo/logo.png -flip ../../../projects/breaking-bad-ds/models/logo/logo.png &&
convert ../../../projects/breaking-bad-ds/models/logo/logo_text.png -flip ../../../projects/breaking-bad-ds/models/logo/logo_text.png &&
convert ../../../projects/breaking-bad-ds/models/logo/backdrop.png -flip ../../../projects/breaking-bad-ds/models/logo/backdrop.png 