
# Path to Matlab installation folder:
MATLABHOME=/Applications/MATLAB704

# Definition of target operating system:
TARGETOS=-DMACOSX

# Build and include GLM into moglcore:
GLM=-DBUILD_GLM
#GLM=

MEXCC=-I${MATLABHOME}/extern/include \
	${GLM} ${TARGETOS} -DGLEW_STATIC\
	-DMATLAB_MEX_FILE -DNDEBUG -fno-common -fexceptions -O3 \
	-no-cpp-precomp -bundle -undefined suppress -flat_namespace \
	-exported_symbols_list ${MATLABHOME}/extern/lib/mac/mexFunction.map

MEXLD=-L${MATLABHOME}/bin/mac -lmx -lmex -lmat -lstdc++ \
	${MATLABHOME}/extern/src/mexversion.c

OGLFLAGS=-framework AGL -framework ApplicationServices -framework Carbon \
	-framework OpenGL -framework IOKit -framework GLUT

CC=gcc


all:  moglcore.mexmac

moglcore.mexmac:  moglcore.o glm.o gl_auto.o gl_manual.o glew.o
	$(CC) ${MEXCC} ${MEXLD} ${OGLFLAGS} \
		-o moglcore.mexmac moglcore.o glm.o gl_auto.o gl_manual.o glew.o
	cp moglcore.mexmac ../core

.c.o:
	$(CC) -c ${MEXCC} -o $*.o $*.c

clean:
	-rm *.o *.mexmac

