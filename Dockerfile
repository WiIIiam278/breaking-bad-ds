FROM devkitpro/devkitarm

RUN git clone https://github.com/RetroVNDjinn/libfilesystem.git; \
    cd /libfilesystem; \
    make; \
    cp ./lib/libfilesystem.a /opt/devkitpro/libnds/lib/libfilesystem.a; \
    cd /;

RUN git clone https://github.com/AntonioND/nitro-engine.git; \
    ln -sT /nitro-engine /opt/devkitpro/nitro-engine; \
    cd /nitro-engine; \
    make; \
    cd /;

RUN git clone https://github.com/knightfox75/nds_nflib.git; \
    ln -sT /nds_nflib /opt/devkitpro/nflib; \
    cd /nds_nflib; \
    make; \
    cd /;

ENTRYPOINT [ "/bin/bash" ]