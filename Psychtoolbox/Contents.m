% Psychtoolbox.
% Version 3.0.22      27 May 2025
%
% The Psychophysics Toolbox is a collection of Octave and Matlab functions
% that extend the language to give you exquisite control of your computer
% to test observers with research-grade dynamic stimuli. Parts of it are
% also used as critical parts of Python toolkits like PsychoPy.
%
% Every Psychtoolbox function has its own documentation available through
% the Matlab HELP command, and, in the case of MEX files, through the
% function itself. We've tried hard to make these individual bits of
% documentation clear, accurate, and complete. We're grateful to receive
% corrections.
%
% We've mostly followed Mathworks's help-text conventions, but note that in
% our help text we designate optional arguments to function calls by
% embracing them with square brackets. You're not meant to include these
% brackets when you actually call the function. For example, "help Snd"
% will tell you this:
%
% err = Snd(command,[sig],[rate])
%
% meaning that the "command" argument is required and the "sig" and "rate"
% arguments are optional. A typical call to Snd looks like this:
%
% Snd('Play', mysoundsignal)
%
% The most important and generally useful functions, especially Screen, are
% in PsychBasic. The PsychDemos will show you how to do useful things in
% Matlab with the Psychtoolbox. Try these:
%
% help PsychDemos
% help PsychBasic
% help Screen
% Screen OpenWindow?
%
% The folder PsychDocumentation contains bits of documentation about
% technical implementation details, how to solve specific tasks and how to
% troubleshoot common problems.
%
% To read more, look at the "intro" and "docs" pages at the web site. The
% web site also has advice about getting help, the Psychtoolbox forum, and
% the latest information about bugs and new releases:
%
% http://psychtoolbox.org/
%
% Peter Scarfe has written a large set of nice and beginner friendly
% Psychtoolbox tutorial scripts.
%
% You can find them at http://peterscarfe.com/ptbtutorials.html
%
% If you want to acknowledge use of this software when you publish your
% research, you might say something like this, "We wrote our experiments in
% Matlab, using the Psychophysics Toolbox extensions (Kleiner et al, 2007)"
%
% Kleiner M, Brainard D, Pelli D, 2007, "What's new in Psychtoolbox-3?"
% Perception 36 ECVP Abstract Supplement.
%
%
%
% Folders and their contents:
%
% PsychAlpha             - Under development. Experimental, risky, undocumented.
% PsychAlphaBlending     - OpenGL alpha-channel blending utilities and constants.
% PsychBasic             - Basic support routines for psychophysics.
% PsychCal               - Calibrate your video monitors.
% PsychCalDemoData       - Demo calibration data.
% PsychColorimetric      - Colorimetric calculations.
% PsychColorimetricData  - Standard colorimetric data.
% PsychContributed       - Contributed programs.
% PsychDemos             - Show how to use the Psychtoolbox.
% PsychDocumentation     - Documentation about specific topics.
% PsychFiles             - Process text files.
% PsychGamma             - Fit monitor gamma functions.
% PsychGLImageProcessing - Built-in image processing via graphics hardware.
% PsychGPGPU             - General purpose accelerated computing on GPUs.
% PsychHardware          - Interface to plug-in hardware.
% PsychInitialize        - Initialize and deinitialize the toolbox.
% PsychObsolete          - Obsolete routines, still present for compatibility.
% PsychOpenGL            - Routines for low-level access to OpenGL 3D graphics.
% Psychometric           - Psychometric function fitting.
% PsychOneliners         - Trivial, but handy, functions.
% PsychOptics            - Optics calculations, mostly for human optics.
% PsychPriority          - Priority and Rush.  (formerly within PsychBasic)
% PsychProbability       - Probability and statistics.
% PsychRadiometric       - Radiometric/photometric calculations and conversions.
% PsychRects             - Manipulate rectangles for drawing.
% PsychSignal            - Signal processing and math routines.
% PsychSound             - Sound input and output routines.
% PsychStairCase         - Adaptive staircase procedure.
% PsychTests             - Evaluate performance of software and hardware.
% PsychVideoCapture      - Functions for realtime video capture.
% Quest                  - Threshold estimation procedure.
%
%
% IMPORTANT NEWS:
%
% Psychtoolbox mex files for GNU Octave and Mathworks Matlab for Apple
% macOS and Microsoft Windows, as well as the Matlab mex files for GNU
% Linux, will require a paid software license key to work. New first time
% installations of Psychtoolbox on a machine will work for free for up to
% 14 days after first use. After that time period, a paid subscription
% license with associated license key will be required.
%
% Psychtoolbox on these system configurations will connect to our network
% licensing servers after installation and after you have given consent, to
% enable you to use it. For more information about our license management,
% type 'help PsychLicenseHandling'.
%
