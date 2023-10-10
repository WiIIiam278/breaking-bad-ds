#!/bin/sh

for file in *.png; do
    grit $file -ftb -fh! -gTFF00FF -gt -gB8 -m!
    echo "Converting $file"
done

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
    echo "Moving $file"
done

mv *.pal *.img ../../nitrofiles/sprite