This folder contains a copy of the Oculus SDK version 0.5.0.1
for Linux. Copies for OSX and Windows could not get included
due to them exceeding the maximum file size limits of GitHub.

You can find the original files, as well as a copy of the included
Linux SDK on Oculus VR LLC's website:

https://developer.oculus.com/downloads/ if one selects
"Platform: PC" and Version "0.5.0.1-beta".

The files are included to simplify setup of new Psychtoolbox
build systems. They are included with permission of the Oculus
license version 3.2, whose text can be found online at:

https://developer.oculus.com/licenses/pc-3.2/

Additionally the license is included in this folder as License.txt,
and in each copy of the SDKs.

Use to build PsychOculusVRCore mex files for different OSes:

Linux: Unpack the SDK into a folder, follow the build instructions.
The built SDK will install into the /usr/local/include and /usr/local/lib
folders as header files and a static library which will get statically
linked into the PsychOculusVRCore mex files for Linux.

OSX: Unpack the SDK into a folder. Then copy the included LibOVR.framework
into the /Library/Frameworks/ folder as LibOVR.framework. The resulting
mex file will get statically linked against a LibOVR shim lib which will
later dynamically load the OSX runtime on the actual HMD computer.

Windows: Unpack the SDK into the same parent folder in which the
Psychtoolbox-3 root folder is located, and rename it to OculusSDKWin,
e.g.,

/home/kleinerm/projects/OpenGLPsychtoolbox/Psychtoolbox-3/
/home/kleinerm/projects/OpenGLPsychtoolbox/OculusSDKWin/

-> Psychtoolbox-3 and OculusSDKWin are siblings in the OpenGLPsychtoolbox
parent folder.

Again, the resulting mex file is statically linked against some LibOVR
shim, which will runtime load/link against the installed 0.5 runtime.

