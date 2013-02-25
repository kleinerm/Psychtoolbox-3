% Psychtoolbox:PsychAnsiZ136MPE.
%
% Partial implementation of Ansi Z136.1-2007 standard for computing
% maximum permissable exposure.
%
% NOTE: FEBRUARY 2013.  THIS IS UNDER DEVELOPMENT AND HAS NOT BEEN FULLY TESTED
%
% help Psychtoolbox              % For an overview, triple-click me & hit enter.
%
%   AnsiZ136MPEComputeCa - Compute constant Ca, Table 6
%   AnsiZ136MPEComputeCb - Compute constant Cb, Table 6
%   AnsiZ136MPEComputeCc - Compute constant Cc, Table 6
%   AnsiZ136MPEComputeCe - Compute constant Ce, Table 6
%   AnsiZ136MPEComputeExtendedSourceLimit - Compute overall MPE limit for extended sources, Table 5b
%   AnsiZ136MPEComputeExtendedSourcePhotochemicalLimit - Compute photochemical MPE for extended sources, Table 5b
%   AnsiZ136MPEComputeLimitingConeAngle - Compute limiting cone angle, Table 6
%   AnsiZ136MPEComputeT2 - Compute constant T2, Table 6
%   AnsiZ136MPETest      - Test the suite of routines.  Generates many figures that should match those in the standard.

% Copyright (c) 2013 by David Brainard