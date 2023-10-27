As of Psychtoolbox 3.0.19.4, this folder only contains files needed for the current builds.

libportaudio_osx_64.a      = Static link library for building PsychPortaudio for 64-Bit Intel macOS with static linking.
portaudio_x64.lib          = Import library used for building PsychPortaudio.mexw64 for 64-Bit Matlab on Windows.
MinGW64/portaudio_x64.lib  = Import library used for building PsychPortaudio.mex for 64-Bit Octave-4.4+ on Windows.

These files are built from upstream Portaudio V19.7.0+ (https://portaudio.com).
For the Windows variants, any trace of ASIO support code has been removed
from the upstream source before building, to have a completely ASIO free build.

The corresponding ASIO-less portaudio_x64.dll Windows runtime library
is in the Psychtoolbox/PsychSound folder as usual. It is build with MME,
WDMKS and WASAPI backends, and without DirectSound or ASIO backends.

On the Linux side, we dynamically build and link against the distro
installed Portaudio shared libraries, which are of version 19.7.0+
on recent distros.
