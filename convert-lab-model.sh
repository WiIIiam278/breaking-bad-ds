cd ../../nitro-engine/tools/obj2dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/superlab/superlab.obj --texture 256 256 --output ../../../projects/breaking-bad-ds/nitrofiles/model/superlab.dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/superlab/superlab_mixer.obj --texture 256 256 --output ../../../projects/breaking-bad-ds/nitrofiles/model/superlab_mixer.dl &&
python obj2dl.py --input ../../../projects/breaking-bad-ds/models/superlab/superlab_pipe.obj --texture 256 256 --output ../../../projects/breaking-bad-ds/nitrofiles/model/superlab_pipe.dl &&
cd ../img2ds &&
# use imagemagick to flip superlab_textures.png ONLY VERTICALLY
convert ../../../projects/breaking-bad-ds/models/superlab/superlab_textures.png -flip ../../../projects/breaking-bad-ds/models/superlab/superlab_textures.png &&
python img2ds.py --input ../../../projects/breaking-bad-ds/models/superlab/superlab_textures.png --name superlab --format A1RGB5 --output ../../../projects/breaking-bad-ds/nitrofiles/model &&
#  flip superlab_textures.png back
convert ../../../projects/breaking-bad-ds/models/superlab/superlab_textures.png -flip ../../../projects/breaking-bad-ds/models/superlab/superlab_textures.png 