lib_LTLIBRARIES = libnvstusb.la
libnvstusbdir=$(includedir)/libnvstusb
libnvstusb_la_SOURCES = nvstusb.c usb_libusb.c
libnvstusb_la_CPPFLAGS = -I@top_srcdir@/include ${LIBUSB_CFLAGS}
libnvstusb_la_LIBS = ${LIBUSB_LIBS} ${X11_LIBS}
libnvstusb_HEADERS = @top_srcdir@/include/usb.h @top_srcdir@/include/nvstusb.h
