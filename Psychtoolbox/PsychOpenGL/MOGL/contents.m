% CONTENTS.M  Contents of mogl toolbox
%
% mogl is a collection of M-File wrappers and a MEX file that allow to call
% all OpenGL commands from Matlab as one is used to from the C programming
% language.
%
%
% The 'OpenGL for Matlab' low level interface wrapper mogl was developed,
% implemented and generously contributed to Psychtoolbox under the GPL
% license by Prof. Richard F. Murray, University of York, Canada.
%
% Directory structure is as follows: 
% 
%     mogldemo.m          -- demonstration of how to use the mogl toolbox
%     mogldemo.mat        -- data used by mogldemo.m
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
%         headers/*.h     -- OpenGL headers
%         private/*.m     -- miscellaneous helper files for autocode.m
% 
%         (second group:  files that compile to produce moglcore.mexmac)
% 
%         gl_auto.c       -- automatically generated interfaces to OpenGL functions
%         gl_manual.c     -- manually generated interfaces to OpenGL functions
%         glm.c           -- GLM library of GLUT-like functions
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
