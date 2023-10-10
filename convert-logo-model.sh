cd ../../nitro-engine/tools/obj2dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/logo/logo.obj --texture 128 128 --output ../../../projects/breaking-bad-ds/nitrofiles/model/logo.dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/logo/logo_text.obj --texture 128 128 --output ../../../projects/breaking-bad-ds/nitrofiles/model/logo_text.dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/logo/logo_skybox.obj --texture 256 256 --output ../../../projects/breaking-bad-ds/nitrofiles/model/logo_skybox.dl &&
cd ../img2ds &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/logo/logo.png --name logo --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/logo/logo_text.png --name logo_text --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/logo/logo_skybox.png --name logo_skybox --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model