% Psychtoolbox:PsychISO2007MPE.
%
% Partial and in progress implementation of ISO 2007 standard for computing
% maximum permissable exposure to broadband lights.
%
% *****************************************************************
% IMPORTANT:
%   a) Individuals using these routines must accept full responsibility 
%   for light exposures they implement. We recommend that values computed
%   with these routines be carefully checked against independent calculations.
%   We have done our best to follow the standard, but it is very complex and
%   there may be errors.
%
%   b) As of June, 2013, these routines are still very much a work
%   in progress and should thus be treated with special caution.  See
%   ISO2007MPEBasicTest.
%
% REFERENCES.
%   Ansi ISO 15004-2, Ophthalmic instruments - Fundamental requirements and test methods -
%   Part 2: Light hazard protection.  The standard document.
%
%   ISO2007MPEBasicTest  - Test the suite of routines.  Generates many figures that should match those in the standard.
%   ISO2007MPEGetWeigthtings - Get the spectral weighting functions needed by the standard.
%
%   ISO2007MPETableA1.txt    - Table A1 of the standard as tab delimited text.
%   ISO2007MPETableA2.txt    - Table A2 of the standard as tab delimited text.


% Copyright (c) 2013 by David Brainard
