function oldBit=StoreBit(newBit)

% oldBit=StoreBit([newBit])
%
% Return the previously stored bit and, optionally, store a new bit.
%
% StoreBit C source demonstrates how to call the OS X Cocoa API from a
% mex file. StoreBit is not a practical means of storing bits in
% MATLAB.    
%
% Because storing a bit in Cocoa is trivial, StoreBit's C source is the
% almost-minimal architecture needed to call Cocoa from MATLAB.
%
% The source code for StoreBit is divided between two targets:
%
%  1. StoreBit, within the main Psychtoolbox Xcode project, which builds
%  StoreBit.mexmac.  StoreBit.mexmac does not contain Objective-C or 
%  Cocoa calls.  It loads the Cocoa bundle StoreBitLib, which does.  
%
%  2. StoreBitLib, within the eponymous Xcode project StoreBitLib, which
%  builds the Cocoa bundle StoreBitBundle.  The source includes
%  Objective-C wrapped within C. StoreBit.mexmac loads StoreBitLib and
%  invokes the C wrapper functions.
% 

% HISTORY
%
% 9/07/05 awi Wrote it.
% 2/01/06 awi Improved description.   

PsychAssertMex;
