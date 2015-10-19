bin_PROGRAMS = example
example_SOURCES = 3dv.c
example_CFLAGS = -I@top_srcdir@/include ${ILUT_CFLAGS} ${IL_CFLAGS} ${GL_CFLAGS}
example_LDADD = @top_builddir@/src/libnvstusb.la ${ILUT_LIBS} ${IL_LIBS} ${GL_LIBS} -lglut ${LIBUSB_LIBS} ${X11_LIBS} -lm

