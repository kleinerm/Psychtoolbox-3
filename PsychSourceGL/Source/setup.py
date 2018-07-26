# setup.py -- Build-Script for building Psychtoolbox-3 "mex" files as Python extensions.
#
# (c) 2018 Mario Kleiner - All rights reserved.
#

from distutils.core import setup, Extension # Build system.
import os, fnmatch                          # Directory traversal, file list building.
import platform                             # OS detection.
import sys                                  # cpu arch detection.
import numpy                                # To get include dir on macOS.

def get_sourcefiles(path):
    sources = [];
    pattern = '*.c';
    for filename in os.listdir(path):
        if fnmatch.fnmatch(filename, pattern):
            sources += [os.path.join(path,filename)];

    # Fancy schmanzi, not needed atm. for recursive dir traversal:
    #    for root, dirs, files in os.walk(path):
    #        for filename in files:
    #            sources += [os.path.join(root,filename)];

    return(sources);

def get_basemacros(name, osname):
    return([('PTBMODULE_' + name, None), ('PTBMODULENAME', name)] + base_macros);

def get_baseincludedirs(name, osname):
    return(['Common/' + name] + baseincludes_common + [osname + '/Base'] + [osname + '/' + name]);

def get_basesources(name, osname):
    extrafiles = [];
    if os.access('./' + osname + '/' + name, os.F_OK):
         extrafiles = get_sourcefiles('./' + osname + '/' + name);

    return(basefiles_common + get_sourcefiles('./' + osname + '/Base/') + get_sourcefiles('./Common/' + name) + extrafiles);

# Treating some special cases like Octave seems to be the right thing to do,
# PSYCH_LANGUAGE setting is self-explanatory:
base_macros = [('PTBOCTAVE3MEX', None), ('PSYCH_LANGUAGE', 'PSYCH_PYTHON')];

# Common infrastructure and the scripting glue module for interfacing with the Python runtime:
basefiles_common = get_sourcefiles('./Common/Base') + ['Common/Base/PythonGlue/PsychScriptingGluePython.c'];
baseincludes_common = [numpy.get_include(), 'Common/Base', 'Common/Screen'];

is_64bits = sys.maxsize > 2**32;

# OS detection and file selection for the different OS specific backends:
print('Platform reported as: %s\n' % platform.system());
if platform.system() == 'Linux':
    # Linux specific backend code:
    print('Building for Linux...\n');
    osname = 'Linux';
    # All libraries to link to all modules:
    base_libs = ['c', 'rt'];
    # No "no reproducible builds" warning:
    base_compile_args = ['-Wno-date-time'];
    # Extra OS specific libs for PsychPortAudio:
    audio_libs = ['asound'];

    if is_64bits == True:
        audio_objects = ['../Cohorts/PortAudio/libportaudio64Linux.a'];
    else:
        audio_objects = ['../Cohorts/PortAudio/libportaudio32Linux.a'];

    # libusb includes:
    usb_includes = ['/usr/include/libusb-1.0'];
    # Extra OS specific libs for PsychHID:
    psychhid_includes = usb_includes;
    psychhid_libs = ['dl', 'usb-1.0', 'X11', 'Xi', 'util'];
    psychhid_extra_objects = [];

if platform.system() == 'Windows':
    print('Building for Windows...\n');
    osname = 'Windows';
    # FIXME All libraries to link to all modules:
    base_libs = ['c', 'rt'];
    # FIXME: No "no reproducible builds" warning:
    base_compile_args = ['-Wno-date-time'];
    # Extra OS specific libs for PsychPortAudio:
    audio_libs = ['FIXME'];
    audio_objects = ['/usr/local/lib/libportaudio.a'];
    # FIXME libusb includes:
    usb_includes = ['/usr/include/libusb-1.0']
    # FIXME Extra OS specific libs for PsychHID:
    psychhid_includes = usb_includes;
    psychhid_libs = ['dl', 'usb-1.0', 'X11', 'Xi', 'util'];
    psychhid_extra_objects = [];

if platform.system() == 'Darwin':
    print('Building for macOS...\n');
    osname = 'OSX';
    # These should go to extra_link_args in Extension() below, but apparently distutils
    # always appends the extra_link_args at the end of the linker command line, which is
    # wrong for -framework's, as they must be stated *before* the .o object files which
    # want to use functions from them. A solution to this problem doesn't exist in distutils
    # almost two decades after the debut of Mac OSX, because hey, take your time!
    #
    # The hack is to set the LDFLAGS environment variable to what we need, as LDFLAGS
    # apparently gets prepended to the linker invocation arguments, so -framework statements
    # precede the .o'bjects they should apply to and the linker is happy again.
    #
    # Downside is that now we have to pass the union of *all* -framework switches ever
    # used for *any* extension module, as os.environ can't be changed during the build
    # sequencing for a distribution package.
    #
    # Is this awful? Absolutely! And i thought the Octave/Matlab build process on macOS
    # sucked big time, but apparently somebody in Pythonland shouted "Hold my beer!" :(
    # Hopefully some new info about this issue will prove me wrong, and there is a sane
    # and elegant solution, but this is the best i could find after hours of Googling and
    # trying.
    #
    # The following would be the full list of frameworks, but apparently including -framework Carbon
    # is enough. Maybe a catch-all including all other frameworks?
    #
    # -framework Carbon -framework CoreServices -framework CoreFoundation -framework CoreAudio -framework AudioToolbox -framework AudioUnit
    # -framework ApplicationServices -framework OpenGL -framework CoreVideo -framework IOKit -framework SystemConfiguration
    # -framework CoreText -framework Cocoa
    #
    os.environ['LDFLAGS'] = '-framework Carbon -framework CoreAudio'
    base_libs = [];

    # No "no reproducible builds" warning:
    base_compile_args = ['-Wno-date-time', '-mmacosx-version-min=10.11'];

    # Extra OS specific libs for PsychPortAudio:
    audio_libs = [];
    # Include our statically linked on-steroids version of PortAudio:
    audio_objects = ['../Cohorts/PortAudio/libportaudio_osx_64.a'];

    # Include Apples open-source HID Utilities for all things USB-HID device handling:
    psychhid_includes = ['../Cohorts/HID_Utilities_64Bit/', '../Cohorts/HID_Utilities_64Bit/IOHIDManager'];
    psychhid_libs = [];
    # Extra objects for PsychHID - statically linked HID utilities:
    psychhid_extra_objects = ['../Cohorts/HID_Utilities_64Bit/build/Release/libHID_Utilities64.a'];


# GetSecs module: Clock queries.
name = 'GetSecs';
GetSecs = Extension(name,
                    extra_compile_args = base_compile_args,
                    define_macros = get_basemacros(name, osname),
                    include_dirs = get_baseincludedirs(name, osname),
                    sources = get_basesources(name, osname),
                    libraries = base_libs,
                   )

# WaitSecs module: Timed waits.
name = 'WaitSecs';
WaitSecs = Extension(name,
                     extra_compile_args = base_compile_args,
                     define_macros = get_basemacros(name, osname),
                     include_dirs = get_baseincludedirs(name, osname),
                     sources = get_basesources(name, osname),
                     libraries = base_libs
                    )

# PsychPortAudio module: High precision, high reliability, multi-channel, multi-card audio i/o.
name = 'PsychPortAudio';
PsychPortAudio = Extension(name,
                           extra_compile_args = base_compile_args,
                           define_macros = get_basemacros(name, osname),
                           include_dirs = get_baseincludedirs(name, osname),
                           sources = get_basesources(name, osname),
                           libraries = base_libs + audio_libs,
                           extra_objects = audio_objects
                          )

# PsychHID module: Note the extra include_dirs and libraries:
name = 'PsychHID';
PsychHID = Extension(name,
                     extra_compile_args = base_compile_args,
                     define_macros = get_basemacros(name, osname),
                     include_dirs = get_baseincludedirs(name, osname) + psychhid_includes,
                     sources = get_basesources(name, osname),
                     libraries = base_libs + psychhid_libs,
                     extra_objects = psychhid_extra_objects
                    )

# IOPort module:
name = 'IOPort';
IOPort = Extension(name,
                   extra_compile_args = base_compile_args,
                   define_macros = get_basemacros(name, osname),
                   include_dirs = get_baseincludedirs(name, osname),
                   sources = get_basesources(name, osname),
                   libraries = base_libs
                  )

setup (name = 'Psychtoolbox4Python',
       version = '0.1',
       description = 'This is the prototype of a port of Psychtoolbox-3 mex files to Python extensions.',
       package_dir = {'' : '../../Psychtoolbox/PsychPython'},
       py_modules = ['psychtoolboxclassic', 'ppatest', 'hidtest'],
       ext_modules = [WaitSecs, GetSecs, IOPort, PsychHID, PsychPortAudio]
      )
