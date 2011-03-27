% CONTENTS.M  Contents of mogl toolbox
%
% mogl is a collection of M-File wrappers and a MEX file that allow to call
% all OpenGL commands from Matlab as one is used to from the C programming
% language.
%
%
% The initial OS/X PowerPC version of the 'OpenGL for Matlab' low level
% interface wrapper mogl was developed, implemented and generously
% contributed to Psychtoolbox under the GPL license by Prof. Richard F.
% Murray, University of York, Canada. Porting to other operating systems
% and architectures, OpenGL 2.x support, and further extensions and
% maintenance has been done by Mario Kleiner.
%
% The code has been relicensed by Richard Murray and Mario Kleiner to the
% more permissive MIT license since 2011.
%
% Directory structure is as follows: 
% 
%     core/
% 
%         (first group:  main toolbox functions)
% 
%         moglcore.mexmac -- main MEX interface to OpenGL functions
%         oglconst.mat    -- constants used by OpenGL routines
%         setupdate.sh    -- shell script to start or stop 'update' process
%                            (normally called via wrap/glmSetUpdate.m)
% 
%         (second group:  miscellaneous helper files)
% 
%         edittag.m       -- edit all M-files that contain a given string
%         finish.m        -- automatically run when quitting MATLAB
%         mor.m           -- bitwise OR of multiple input arguments
% 
%     source/
% 
%         (first group:  files that generate interface code)
%         
%         autocode.m      -- MATLAB script that generates gl_auto.c and M-file
%                            interfaces to moglcore.mexmac
%         autono.txt      -- list of OpenGL functions that autocode.m should not
%                            generate interfaces for;  most of these appear
%                            in gl_manual.c
%         gl_auto_init.c  -- file used in generating gl_auto.c;  contains
%                            top portion of file, i.e., #includes, etc.
%         oglconst.m      -- MATLAB script that searches through OpenGL header
%                            files for #defined constants, and writes them
%                            to oglconst.mat as variables
%         headers/*.h     -- OpenGL headers to parse in addition to system
%                            header files.
%         private/*.m     -- miscellaneous helper files for autocode.m
% 
%         (second group:  files that compile to produce moglcore.mexmac)
% 
%         gl_auto.c       -- automatically generated interfaces to OpenGL functions
%         gl_manual.c     -- manually generated interfaces to OpenGL functions
%         glm.c           -- GLM library of GLUT-like functions - not build
%                            by default -- deprecated.
%         moglcore.c      -- main MEX interface function
%         mogltypes.h     -- useful data types
%         makefile        -- makefile to compile C files into moglcore.mexmac
% 
%     wrap/*          -- wrapper M-files that check arguments, etc., and
%                        then call to moglcore.mexmac to run OpenGL functions
% 
%
% The following three commands will completely regenerate mogl.
% 
% >> autocode     % generate gl_auto.c and wrapper M-files
% >> ! make       % compile C code to produce MEX files
% >> oglconst     % save constants from header files in a .mat file

% 06-Jan-2006 -- created (RFM)
% 27-Mar-2011 -- edited (MK)
% 27-Mar-2011 -- Update info about license - New MIT license (MK)
