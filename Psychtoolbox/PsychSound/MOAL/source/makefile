
# Path to Matlab installation folder:
MATLABHOME=/Applications/MATLAB72

# Definition of target operating system:
TARGETOS=-DMACOSX

# Build and include GLM into moglcore:
GLM=-DBUILD_GLM
#GLM=

MEXCC=-I${MATLABHOME}/extern/include \
	${GLM} ${TARGETOS} \
	-DMATLAB_MEX_FILE -DNDEBUG -fno-common -fexceptions -O3 \
	-no-cpp-precomp -bundle -undefined suppress -flat_namespace \
	-exported_symbols_list ${MATLABHOME}/extern/lib/mac/mexFunction.map

MEXLD=-L${MATLABHOME}/bin/mac -lmx -lmex -lmat -lstdc++ \
	${MATLABHOME}/extern/src/mexversion.c

OGLFLAGS=-framework ApplicationServices -framework Carbon \
	-framework OpenAL

CC=gcc

all:  moalcore.mexmac

moalcore.mexmac:  moalcore.o moaltypes.h alm.o al_auto.o al_manual.o
	$(CC) ${MEXCC} ${MEXLD} ${OGLFLAGS} \
		-o moalcore.mexmac moalcore.o alm.o al_auto.o al_manual.o
	cp moalcore.mexmac ../core

.c.o:
	$(CC) -c ${MEXCC} -o $*.o $*.c

clean:
	-rm *.o *.mexmac

