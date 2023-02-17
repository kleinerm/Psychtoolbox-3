This subfolder libusb1-win32 contains the precompiled binary snapshot
and source code of the current libusb-1.0 for MS-Windows snapshot, as
Well as a copy for Apple macOS, built on macOS 11.6.1 with assumed
deployment target macOS 11.

libusb-1.0.26-binaries.7z contains the precompiled binary snapshot for
libusb v1.0.26, downloaded at 11-November-2022, from
https://github.com/libusb/libusb/releases/download/v1.0.26/libusb-1.0.26-binaries.7z


libusb-1.0.26.tar.bz2, downloaded at 11-November-2022, from
https://github.com/libusb/libusb/releases/download/v1.0.26/libusb-1.0.26.tar.bz2
contains the corresponding snapshot of the git source tree for the binaries.

Some of the contents of the binary zip file, specifically the libusb.h
header include file, the libusb-1.0.dll runtime / link library DLL's,
and the corresponding import libraries, all in variants for MinGW
and MSVC 2015+, both 32-Bit Intel and 64-Bit Intel, are also extracted
into this folder / subfolders, to allow rebuilding PsychHID et al. for
32-Bit / 64-Bit Octave (MinGW 32/64), 32-Bit / 64-Bit Python (MS32 / MS64)
and 64-Bit Matlab (MS64) subfolders.

The relevant 32-Bit runtime DLL for actual use with 32-Bit versions of
Octave, Matlab R2007a and later, and Python on MS-Windows is stored in the
PTB distro as Psychtoolbox/PsychContributed/libusb-1.0.dll

The relevant 64-Bit runtime DLL for actual use with 64-Bit versions of
Octave, Matlab R2014b and later, and Python on MS-Windows is stored in the
PTB distro as Psychtoolbox/PsychContributed/x64/libusb-1.0.dll

The macOS dylib for building/linking or runtime is stored under libusb-1.0.dylib
in this main folder. It is for 64-Bit Intel cpu architecture only and meant
to support building the PsychHID Python extension without need for new external
dependencies. The Octave and Matlab builds are setup to expect a /usr/local/
Installation, e.g., provided conveniently by HomeBrew. weak_linking is
recommended, so runtime libs are not mandatory on Python for rarely needed
functionality. This is for now mostly for building PsychHID.pyex without failure.

libusb-1.0 is licensed under LGPLv2+. See the file COPYING.txt for a copy
of the license text.

It is dynamically linked against the PsychHID driver for low-level USB
access support, e.g., usb control/interrupt/bulk/... transfers. It is
included and distributed here unmodified from its original source. The
source code is included to satisfy our obligations to distribute the
source code if we include the binaries. If you have any interest in this,
you should probably always get the latest binaries and/or source from the
upstream projects main website...

https://libusb.info

..., as our snapshot included here may be quite old and outdated at any
given point in time. We only rarely upgrade libusb to more recent versions.
E.g., the current upgrade to v1.0.26 happened in November 2022, eleven (!)
years after the previous upgrade to a version older than v1.0.9 !