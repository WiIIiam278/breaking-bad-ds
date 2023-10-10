#!/bin/sh

for file in *.png; do
    grit $file -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs
    echo "Converting $file"
done

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
    echo "Moving $file"
done

mv *.pal *.img *.map ../../nitrofiles/bg