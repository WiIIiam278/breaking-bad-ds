for i in *.ogg; do ffmpeg -i "$i" -y -f s8 -acodec pcm_s8 -ar 11025 -vn "../../nitrofiles/bgm/${i%.*}.raw"; done