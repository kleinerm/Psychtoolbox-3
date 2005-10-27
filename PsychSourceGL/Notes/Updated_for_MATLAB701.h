/*
	Updated_for_MATLAB701.h
	
	PLATFORMS:
	
		Only OS X
		
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY:
	
		11/2/04		awi		Wrote it.
		1/10/05		awi		Enhanced Description section to say explain why you might want to read this.  
	 	
	DESCRIPTION:
	
		This file consists of comments documenting how we updated of the Psychtoolbox Xcode project with Psychtoolbox release 1.0.2
		for new releases of MATLAB and Xcode.  This has already been done for you and changes are emobodied in the Psychtoolbox
		Xcode project; most Psychtoolbox programmers will not need to read this document.   
		
		Information here should be useful again in the future when updating the Psychtoolbox Xcode projects for new releasese of
		MATLAB and Xcode.  Herein we explain how to uncover and transfer build settings use by MATLAB's mex buid scripts into Xcode 
		and set up the Xcode debugger.  


CONTENTS
________

	CONTENTS
	METHODS
	UNCOVERING BUILD SETTINGS FOR MEX FILES IN MATLAB 7.0.1
	TRANSFERING BUILD SETTINGS TO BUILD STYLES AND TARGETS
	UNCOVERING MATLAB DEBUGGER FLAGS
	CONFIGURING THE XCODE DEBUGGER FOR MATLAB

METHODS
-------

******
NOTE: Matlab R14 Service Pack 2 is the most up-to-date release of MATLAB as of 4/11/05.  Psychtoolbox 1.0.5, released on that date, is tested and compiled against the earlier Matlab R14 Service 
Pack 1.  We do not expect this to cause any problems.  We will update to Service Pack 2 for the next Psychtoolbox release.
******  

At OS X Psychtoolbox version 1.0.2 the Psychtoolbox Xcode project was updated to build against MATLAB version 7.0.1 libraries on OS X 10.3.5 using XCode version 1.5.  You 
should have those versions of MATLAB,  OS X and XCode or better to build mex files using this Xcode Psychtoolbox project.  The XCode projects are configured to target OS X version
10.2.x, so in addition to OS 10.3.x the current Psychtoolbox should be compatible with OS 10.2, but we have not tested that.  The Psychtoolbox XCode project and the Psychtoolbox
itself might be compatible with some earlier versions of the required companion software, but we have not tested that, do not suppport it, and recommend that you use
current and up-to-date versions.      
 
When upgrading to a new version of MATLAB it is sometimes necessary to change the Project settings for the Psychtoolbox XCode project which builds Psychtoolbox mex files.
This is because: 1. The GCC compiler and linker flags used when building mex files have to be compatible with (unknown) flags which MathWorks used to build MATLAB.
2. The XCode project accesses resources within the MATLAB directly, such as header and library files, and needs to know the paths into the MATLAB directly to those
resources.  The paths change between MATLAB versions, at least for the reason that the MATLAB root directory name changes with each MATLAB release.   
   
The easiest way to uncover settings needed to build mex files in Xcode is to invoke MATLAB's built-in C compiler,  "mex", with the verbose flag, "-v".  Mex in verbose mode will
display gcc compiler flags which you can then then transfer to XCode.  This works because both mex and Xcode ultimately rely on gcc to build mex files.  The mex compiler 
is a rubegoldbergian chain of scripts: a .m file which invokes a shell script which generates compiler flags appropriate for your system, passed to another script
which invokes the gcc liker and compiler.  Initially we tried to uncover the gcc compiler flags by reading MathWorks build scripts, but that is really hard. It is much easier 
to get flags from building in verbose mode than to attempt to glean settings from reading the mex and shell build scripts.   

Given a GCC flag, it was really difficult to locate the corresponding setting in Proeject Builder.  That's because Proeject Builder had its own names for build settings which did
not correspond to gcc flags in any obvious or documented way.  Fortunately, the situation is much improved in Xcode with clearly marks all build settings to indicate if they 
control gcc flags and actually name the corresponding flag. Yay!

The procedure to setup Xcode to build a mex file is:
1.  From MATLAB invoke the mex compiler on the timestwo.c example, "mex -v -g timestwo.c".  It's helfpul to move timestwo.c to the MATLAB path so that MATLAB can find the
timestwo.mexmac build product to test.  You have specify the full path to timestwo.c or place it in the MATLAB root directory because mex does not seem to know that it should
search the MATLAB path for the C file which you tell it to compile.   

2. If you want to start from scratch, which you don't, you would create a Carbon library project in XCode.  Instead you probably want to update an existing XCode Psychtoolbox target in XCode, 
in which case, just start with that.                

3. If you are upgrading to a new XCode: In XCode Project menu select "Update All Targets in Project to Native" or select targets one-by-one and "Upgrade to Native Target". 
Xcode will not inform you that you need to update targets but will silently screw up if the targets are not updated.  If you don't update, the target settings and build style 
settings will not set gcc build flags according to your settings.  In particular, I caught it inserting the flag -mdynamic-no-pic into the the gcc build style though project and
target settings indicated that the setting was controlled from the build settings and turned off (-mdynamic-no-pic is incompatible with dynamic libraires).  Do not fear target updates:
Updating a target is reversible because XCode retains the old target withing the project and inserts a new updated one named after the old one.  

4. Look at the gcc build call that mex gave you and attempt to set Xcode build settings to match.  Xcode settings are set in two places:  The project build style settings and the 
target settings.  These overalap to some degree and the build style settings override the target settings.  For more information on how to set XCode settings, look here:
http://developer.apple.com/documentation/DeveloperTools/Conceptual/Build_System/index.html?http://developer.apple.com/documentation/DeveloperTools/Conceptual/Build_System/Trouble_Build_Settings/Trouble_Build_Settings.html


5. If your mex  file  built, try to call the mex file in MATLAB.  Use the "which" command in MATLAB to locate the file which MATLAB will load when you call the command and  check the modify
date of that file in the Finder to make sure that it  is the file which you build and not a pre-existing file.

6. You might have gotten step 4 wrong.  You would know this if the mex file  failed to build  or  failed to execute in MATLAB.  Here is how to check what flags Xcode passed to gcc and 
correct problems: The XCode build window has a detailed build log pane which is collapsed by default.  Find the button on the build window which expands the detailed build log pane and
locate the gcc compile and link calls in the log.  Compare these to what you intened to set.  Note that there will be  more arguments  passed to gcc  than what mex used when invoking gcc.  
You need to idnentify the harmless ones and leave them there.  To find out what a given flag does: 
	¥ check the gcc manual:  http://www.gnu.org/software/gcc/onlinedocs/ 
	¥ Check the build settings and build style windows in XCode which display documenation for Darwin-specific gcc flags if you click on any control prefaced with the C icon.

7.  Repeat steps 4, 5 and 6 as necessary.


The Psychtoolbox Xcode Project now relies on MATLAB being at /Applications/MATLAB701.  Previously the Psychtoolbox project used paths into a custom-made MATLAB 
Application bundle distributed only within the New York University Center for Neural Science. If you wanted to develop in C for the Psychtoolbox you had to change
the project path settings.  We now use and rely on the standard configuration as created by the MATLAB installer provided by MathWorks.  Previously we defined
and environment variable, which XCode read, giving the path into the MATAB root directly.  Now, instead we use $(SYSTEM_APPS_DIR), which by default OS X defines
to by /Applications.  Because the Xcode project now relies on finding MATLAB in  $(SYSTEM_APPS_DIR) (/Applications/MATLAB701) it is no longer necessary to define 
the enviroment variable PSYCHSOURCE_MATLAB_ROOT to insall the Psychtoolbox source.  Now the only step necessary to install the Psychtoolbox source should be to 
change the MATLAB path to include the Psychtoolbox directory inside the source folder, instead of the one inside the Psychtoolbox distribution.  

If you have to place MATLAB somewhere other than than the SYSTEM_APPS_DIR directory, then it might work to create within the Applications directory a file alias 
(from the finder) or a link (ln from the Terminal) , though I have not tried this.  Otherwise, you will have to edit XCode project settings, which is not difficult but is 
less convenient. 



UNCOVERING BUILD SETTINGS FOR MEX FILES IN MATLAB 7.0.1
_______________________________________________________
    

The gcc compiler and linker compiler flags reported by the mex "mex -v -g" command on MATLAB version 7.0.1 are these:

		>> mex -v -g /Users/ingling/Desktop/TestMex/timestwo.c
		-> mexopts.sh sourced from directory (DIR = $MATLAB/bin)
		   FILE = /Applications/MATLAB701/bin/mexopts.sh
		----------------------------------------------------------------
		->    MATLAB                = /Applications/MATLAB701
		->    CC                    = gcc-3.3
		->    CC flags:
				 CFLAGS             = -fno-common -no-cpp-precomp -fexceptions
				 CDEBUGFLAGS        = -g
				 COPTIMFLAGS        = -O3 -DNDEBUG
				 CLIBS              = -L/Applications/MATLAB701/bin/mac -lmx -lmex -lmat -lstdc++
				 arguments          = 
		->    CXX                   = g++-3.3
		->    CXX flags:
				 CXXFLAGS           = -fno-common -no-cpp-precomp -fexceptions
				 CXXDEBUGFLAGS      = -g
				 CXXOPTIMFLAGS      = -O3 -DNDEBUG
				 CXXLIBS            = -L/Applications/MATLAB701/bin/mac -lmx -lmex -lmat -lstdc++
				 arguments          = 
		->    FC                    = f77
		->    FC flags:
				 FFLAGS             = -f -N15 -N11 -s -Q51 -W
				 FDEBUGFLAGS        = -g
				 FOPTIMFLAGS        = -O -cpu:g4
				 FLIBS              = -L/sw/lib -lfio -lf77math
				 arguments          = 
		->    LD                    = gcc-3.3
		->    Link flags:
				 LDFLAGS            = -bundle -Wl,-flat_namespace -undefined suppress -Wl,-exported_symbols_list,/Applications/MATLAB701/extern/lib/mac/mexFunction.map
				 LDDEBUGFLAGS       = -g
				 LDOPTIMFLAGS       = -O
				 LDEXTENSION        = .mexmac
				 arguments          = 
		->    LDCXX                 = 
		->    Link flags:
				 LDCXXFLAGS         = 
				 LDCXXDEBUGFLAGS    = 
				 LDCXXOPTIMFLAGS    = 
				 LDCXXEXTENSION     = 
				 arguments          = 
		----------------------------------------------------------------

		-> gcc-3.3 -c  -I/Applications/MATLAB701/extern/include -I/Applications/MATLAB701/simulink/include -DMATLAB_MEX_FILE -fno-common -no-cpp-precomp -fexceptions  -g /Users/ingling/Desktop/TestMex/timestwo.c

		-> gcc-3.3 -c  -I/Applications/MATLAB701/extern/include -I/Applications/MATLAB701/simulink/include -DMATLAB_MEX_FILE -fno-common -no-cpp-precomp -fexceptions  -g /Applications/MATLAB701/extern/src/mexversion.c

		-> gcc-3.3 -g -bundle -Wl,-flat_namespace -undefined suppress -Wl,-exported_symbols_list,/Applications/MATLAB701/extern/lib/mac/mexFunction.map -o timestwo.mexmac  timestwo.o mexversion.o  -L/Applications/MATLAB701/bin/mac -lmx -lmex -lmat -lstdc++

	
Note that we compiled with the "-g" flag, so we get compiler flags for compiling a debugable executable.  If you wanted to know the flags for compiling an optimized binary, then
leave out the "-g" flag.  



TRANSFERING BUILD SETTINGS TO BUILD STYLES AND TARGETS 
______________________________________________________


Some build parameters were changed in build style settings for the project, which apply to all targets, but we do not mention them in this file.

Other build parameters were changed in the build style settings for every target. This is a list of changes made to build settings for every target.

1. Under Rules change C source files using GCC system version to:

GCC System Version 3.3

2.  Set Header Search Paths:

"$(SYSTEM_APPS_DIR)/MATLAB701/extern/include" "$(SYSTEM_APPS_DIR)/MATLAB701/simulink/include"

3.  Set Library Search Paths:

"$(SYSTEM_APPS_DIR)/MATLAB701/bin/mac"

4. Change Exported Symbols file to:

$(SYSTEM_APPS_DIR)/MATLAB701/extern/lib/mac/mexFunction.map

6. Change Other Linker Flags to:

-bundle -flat_namespace -undefined suppress -lmx -lmex -lmat -lstdc++

5. Add to Other C Flags:

-fexceptions -no-cpp-precomp

6. If it builds, rename  the old target by apending "(Old)" to the name and rename the new target by removing "(Upgraded)" from the name.



UNCOVERING MATLAB DEBUGGER FLAGS
________________________________


When you launch MATLAB in OS X you are not invoking MATLAB directly.  Instead, you are launching a script wich sets environment variables for the MATLAB execution context and 
then launches the MATLAB binary. To configure the XCode debugger to launch MATLAB, we must not tell it to launch MATLAB as usual, or else we will be degugging only the MATLAB launch 
script instead of MATLAB!  A consequence of not running the MATLAB launch script to start MATLAB is that Xcode must also set environment variables which would otherwise be 
set by MATLAB's build script.

To configure Xcode to set enviroment variables for MATLAB you must know what are those variables and their values.  There are two ways to uncover the environment variables which the
MATLAB lanuch script sets.  One way is to read those scripts.  An easier way is to lanuch matlab and then display the environment variables in the shell context in which MATLAB executes
by issueing the shell-escaped "!env" command from the MATLAB command line:

		>> !env
		MANPATH=/sw/share/man:/usr/share/man:/usr/X11R6/man:/usr/local/localHIPS/man:/usr/local/localHIPS/man
		XKEYSYMDB=/Applications/MATLAB701/X11/app-defaults/XKeysymDB
		MATLABPATH=/Applications/MATLAB701/toolbox/local
		MWLibraryFiles=MSL_Runtime_Mach-O.a:MSL_C++_Mach-O.a
		HOST=Tokara
		SHELL=/bin/tcsh
		PERL5LIB=/sw/lib/perl5
		QTDIR=/sw
		OLDPWD=/Applications/MATLAB701
		XAPPLRESDIR=/Applications/MATLAB701/X11/app-defaults
		GROUP=staff
		USER=ingling
		AUTOMOUNT_MAP=/private ->
		CWINSTALL=/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior
		HOSTTYPE=powermac
		KDEDIR=/sw
		__CF_USER_TEXT_ENCODING=0x1F5:0:0
		MWLibraries=/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MacOS X Support/Libraries/Runtime/Libs:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C++/MSL_MacOS/Lib/Mach-O:/usr/lib
		TOOLBOX=/Applications/MATLAB701/toolbox
		PSYCHSOURCE_MATLAB_ROOT=/Applications/MATLAB.app/Contents/Matlab13.1
		PATH=/sw/bin:/sw/sbin:/usr/bin:/bin:/usr/sbin:/sbin:/Users/ingling:/usr/X11R6/bin:/usr/local/localHIPS/hips:/usr/local/localHIPS/hips
		PWD=/Applications/MATLAB701
		QMAKESPEC=/sw/share/qt3/mkspecs/darwin-g++
		XFILESEARCHPATH=/System/Library/Frameworks/JavaVM.framework/lib/locale/%L/%T/%N%S:
		MWFrameworkVersions=System
		BASEMATLABPATH=
		MWFrameworkPaths=/System/Library/Frameworks
		SHLVL=4
		HOME=/Users/ingling
		OSTYPE=darwin
		DYLD_LIBRARY_PATH=/Applications/MATLAB701/sys/os/mac:/Applications/MATLAB701/bin/mac:/Applications/MATLAB701/extern/lib/mac
		VENDOR=apple
		MATLAB=/Applications/MATLAB701
		LOGNAME=ingling
		MACHTYPE=powerpc
		ARCH=mac
		MWCIncludes=/usr/include:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C/MSL_Common/Include:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C/MSL_MacOS/Include:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C++/MSL_COMMON/Include:/usr/include/gcc/darwin/2.95.2
		INFOPATH=/sw/share/info:/sw/info:/usr/share/info
		DISPLAY=:0
		GL_ENABLE_DEBUG_ATTACH=YES
		SECURITYSESSIONID=2112c0
		TERM='MATLAB Command Window'
		>>    

 		
It is not obvious from this listing which of these shell variables were set by the MATLAB launch script and must be transferred to the Xcode debugger setup, and which are set by 
other methods (for example .profile) and you can ignore.  However, its easy to determine: from Terminal window (not the Xterm window) issue the env command:

		SECURITYSESSIONID=2112c0
		HOME=/Users/ingling
		SHELL=/bin/tcsh
		USER=ingling
		PATH=/sw/bin:/sw/sbin:/bin:/sbin:/usr/bin:/usr/sbin:/usr/X11R6/bin:/usr/local/localHIPS/hips
		GL_ENABLE_DEBUG_ATTACH=YES
		PSYCHSOURCE_MATLAB_ROOT=/Applications/MATLAB.app/Contents/Matlab13.1
		__CF_USER_TEXT_ENCODING=0x1F5:0:0
		TERM_PROGRAM=Apple_Terminal
		TERM_PROGRAM_VERSION=100
		TERM=xterm-color
		LOGNAME=ingling
		HOSTTYPE=powermac
		VENDOR=apple
		OSTYPE=darwin
		MACHTYPE=powerpc
		SHLVL=1
		PWD=/Users/ingling
		GROUP=staff
		HOST=Tokara
		CWINSTALL=/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior
		MWCIncludes=/usr/include:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C/MSL_Common/Include:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C/MSL_MacOS/Include:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C++/MSL_COMMON/Include:/usr/include/gcc/darwin/2.95.2
		MWLibraries=/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MacOS X Support/Libraries/Runtime/Libs:/Applications/Metrowerks CodeWarrior 7.0/Metrowerks CodeWarrior/MSL/MSL_C++/MSL_MacOS/Lib/Mach-O:/usr/lib
		MWLibraryFiles=MSL_Runtime_Mach-O.a:MSL_C++_Mach-O.a
		MWFrameworkPaths=/System/Library/Frameworks
		MWFrameworkVersions=System
		MANPATH=/sw/share/man:/usr/share/man:/usr/X11R6/man:/usr/local/localHIPS/man
		INFOPATH=/sw/share/info:/sw/info:/usr/share/info
		PERL5LIB=/sw/lib/perl5
		KDEDIR=/sw
		QMAKESPEC=/sw/share/qt3/mkspecs/darwin-g++
		QTDIR=/sw
		DISPLAY=:0

Take the difference between these sets of variables to find shell variables are set by the MATLAB init script:

		ARCH=mac
		AUTOMOUNT_MAP=/private ->
		BASEMATLABPATH=
		DYLD_LIBRARY_PATH=/Applications/MATLAB701/sys/os/mac:/Applications/MATLAB701/bin/mac:/Applications/MATLAB701/extern/lib/mac
		MATLAB=/Applications/MATLAB701
		MATLABPATH=/Applications/MATLAB701/toolbox/local
		OLDPWD=/Applications/MATLAB701/toolbox/local
		TOOLBOX=/Applications/MATLAB701/toolbox
		XAPPLRESDIR=/Applications/MATLAB701/X11/app-defaults
		XFILESEARCHPATH=/System/Library/Frameworks/JavaVM.framework/lib/locale/%L/%T/%N%S:
		XKEYSYMDB=/Applications/MATLAB701/X11/app-defaults/XKeysymDB
		

MATLAB, when invoked from the command line with the "-n" flag, reports the environment variables on which it relies and names their origin.  You can use this to 
cross-check the list of variables which you already have:

	------------------------------------------------------------------------
			a = argument  e = environment  r = rcfile  s = script
	------------------------------------------------------------------------
	->  r   MATLAB              = /Applications/MATLAB701
	->      LM_LICENSE_FILE     = (variable not defined)
	->      MLM_LICENSE_FILE    = (variable not defined)
	->  r   AUTOMOUNT_MAP       = /private ->
	->  e   DISPLAY             = :0
	->  r   ARCH                = mac
	->  s   TOOLBOX             = /Applications/MATLAB701/toolbox
	->  r   XAPPLRESDIR         = /Applications/MATLAB701/X11/app-defaults
	->  r   XKEYSYMDB           = /Applications/MATLAB701/X11/app-defaults/XKeysymDB
	->  e   MAX_OPEN_FILES      = 256
	->  s   _JVM_THREADS_TYPE   = 
	->  e   MATLAB_JAVA         = 
	->  s   MATLAB_MEM_MGR      = 
	->  s   MATLAB_DEBUG        = 
	->  s   LD_LIBRARY_PATH     = 
	->  a   arglist             = 
	->  e   SHELL               = /bin/tcsh
	->  e   PATH                = /sw/bin:/sw/sbin:/bin:/sbin:/usr/bin:/usr/sbin:/usr/X11R6/bin:/usr/local/localHIPS/hips
	 
	->  s   MATLABPATH          = (initial version)
			/Applications/MATLAB701/toolbox/local
  
This doesn't seem to make sense though; If we exclude what is set by argument and environment:
    
		ARCH                = mac
		AUTOMOUNT_MAP       = /private ->
		MATLAB              = /Applications/MATLAB701
		MATLABPATH          = (initial version) /Applications/MATLAB701/toolbox/local
		TOOLBOX             = /Applications/MATLAB701/toolbox
		XAPPLRESDIR         = /Applications/MATLAB701/X11/app-defaults
		XKEYSYMDB           = /Applications/MATLAB701/X11/app-defaults/XKeysymDB
		
The the resulting list does not agree with what we found earlier by displaying actual environment variables.   Because there is a discrepancy between what "MATLAB -n" and the 
env commmand report and env can not be wrong we believe (modus tollendo ponens) that "MATLAB -n" is wrong;  "MATLAB -n" predicts which variables "MATLAB" will set and predicts
incorrectly.

  
CONFIGURING THE XCODE DEBUGGER FOR MATLAB
_________________________________________

The Xcode project should include a MATLAB executable with setting derminded by methods in the previous section.  Selecting its radio button within Xcode 
and then clicking the Xcode debug button should lanuch Xcode.  MATLAB when launched by the Xcode debugger will stop at break points set within mex files.  

Xcode seems to sometimes lose the MATLAB executable for unknown reasons.  You can set up a MATLAB executable by right-clicking the "Executables" group in the left-hand Xcode
pane and selecting "Add-->New Custom Executable".  Within the dialog box which appears, name the executable "MATLAB" and select its path, by default 
"/Applications/MATLAB701/bin/mac/MATLAB". 

    

  
*/