# setup.py -- Build-Script for building Psychtoolbox-3 "mex" files as Python extensions.
#
# (c) 2018 Mario Kleiner - All rights reserved.
#

from distutils.core import setup, Extension # Build system.
import os, fnmatch                          # Directory traversal, file list building.
import platform                             # OS detection.

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
baseincludes_common = ['Common/Base', 'Common/Screen'];

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
    audio_objects = ['/usr/local/lib/libportaudio.a'];
    # libusb includes:
    usb_includes = ['/usr/include/libusb-1.0'];
    # Extra OS specific libs for PsychHID:
    psychhid_libs = ['dl', 'usb-1.0', 'X11', 'Xi', 'util'];

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
    psychhid_libs = ['dl', 'usb-1.0', 'X11', 'Xi', 'util'];

if platform.system() == 'Darwin':
    print('Building for macOS...\n');
    osname = 'OSX';
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
    psychhid_libs = ['dl', 'usb-1.0', 'X11', 'Xi', 'util'];

# GetSecs module: Clock queries.
name = 'GetSecs';
GetSecs = Extension(name,
                    extra_compile_args = base_compile_args,
                    define_macros = get_basemacros(name, osname),
                    include_dirs = get_baseincludedirs(name, osname),
                    sources = get_basesources(name, osname),
                    libraries = base_libs
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
                     include_dirs = get_baseincludedirs(name, osname) + usb_includes,
                     sources = get_basesources(name, osname),
                     libraries = base_libs + psychhid_libs
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
       ext_modules = [GetSecs, WaitSecs, PsychPortAudio, PsychHID, IOPort]
      )
