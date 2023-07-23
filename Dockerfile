FROM skylyrac/blocksds:dev-latest

RUN git clone https://github.com/AntonioND/nitro-engine.git; \
    cd /nitro-engine; \
    ln -sT /nitro-engine /opt/wonderful/nitro-engine; \
    make -f Makefile.blocksds; \
    make -f Makefile.blocksds NE_DEBUG=1; \
    make -f Makefile.blocksds install; \
    cd /;

RUN git clone https://github.com/knightfox75/nds_nflib.git; \
    cd /nds_nflib; \
    ln -sT /nds_nflib /opt/wonderful/nflib; \
    make -f Makefile.blocksds install; \
    cd /;

ENTRYPOINT ["/bin/bash", "-c", "make"]