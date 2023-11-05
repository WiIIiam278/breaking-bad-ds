# for i in *.ogg; do ffmpeg -i "$i" -y -f s8 -acodec pcm_s8 -ar 11025 -vn "../../nitrofiles/bgm/${i%.*}.raw"; done

# Convert all *ogg files to 16-bit 44.1kHz PCM .wav files in ../../nitrofiles/bgm/file.wav
for i in *.ogg; do ffmpeg -i "$i" -y -f s16le -acodec pcm_s16le -ar 44100 -vn "../../nitrofiles/bgm/${i%.*}.wav"; done