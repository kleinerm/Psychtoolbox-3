ARToolkit libraries:

ARToolkit is free software, licensed under GPL v2.

The PsychCV mex files makes use of ARToolkits image processing and
tracking facilities. The PsychCV mex file is only provided for
use with GNU/Octave, not with Mathworks proprietary Matlab environment.

This folder provides the source code of ARToolkit, as required by the GPL:

In PsychSourceGL/Cohorts/ARToolkit:

* The include subfolder contains the header files which are
included in the build process for PsychCV.mex

* The zip file contains the complete source code, including
precompiled libraries (.lib's and .dll's) and executables
for Windows. You need to copy those lib's and dll's
to the /lib and system32/ folders of your machine to build
Windows PsychCV with linkage.

-> PsychCV links statically against the other AR libs, so only
need to copy those .lib's into your build environments libs
folder, no need for user installation.

-> Some libraries need freely redistributable MSVC runtimes,
which are included in the Psychtoolbox/PsychContributed/ARToolkit/
subfolder for users convenience, should an installation be required.

For OS/X builds, the precompiled static link libraries (.a's) are
stored inside PsychSourceGL/Projects/MacOSX/PsychToolbox/ARToolkitLibs/
These are just compiled via XCode project, stored here for convenience.

==> All content can be derived from the included .zip file!
