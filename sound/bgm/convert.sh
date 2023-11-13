# Converts ogg soundtrack files into metadata-stripped WAVs
for i in *.ogg; do ffmpeg -y -i "$i" -map_metadata -1 -acodec adpcm_ima_wav -ac 2 -ar 22050 -fflags +bitexact -flags:v +bitexact -flags:a +bitexact "../../nitrofiles/bgm/${i%.*}.wav"; done