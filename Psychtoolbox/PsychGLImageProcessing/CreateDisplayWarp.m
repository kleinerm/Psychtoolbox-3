function warpstruct = CreateDisplayWarp(window, calibfilename)
% warpstruct = CreateDisplayWarp(window, calibfilename);
%
% Helper routine for Geometric display undistortion mapping, not to be
% called inside normal PTB scripts!
%
% This function reads a display calibration file 'calibfilename' and builds
% a "geometric warp function" based on the calibration information in
% 'calibfilename' for the onscreen window with handle 'window'. It returns
% a struct 'warpstruct' that defines the created warp function. You can
% pass this 'warpstruct' as a parameter to the Psychtoolbox command...
%
% You normally do not call this routine directly from your script. Its
% called internally by the PsychImaging() command...
% 
% PsychImaging('AddTask', viewchannel, 'GeometryCorrection', calibfilename);
%
% ...in order to setup PTB's imaging pipeline for realtime geometry
% correction, based on the calibration info in the file 'calibfilename'.
%
% Example: You created a calibration file 'mycalib.mat' to undistort the
% left view display of a stereo setup. Then you could apply this
% undistortion function via the following setup code:
%
% PsychImaging('PrepareConfiguration');
% PsychImaging('AddTask', 'LeftView', 'GeometryCorrection', 'mycalib.mat');
% window = PsychImaging('OpenWindow', screenid);
%
% This would open an onscreen window just as window=Screen('OpenWindow', screenid);
% would do. It would configure the window for automatic undistortion based
% on the data in 'mycalib.mat'.
%
% Psychtoolbox provides (or will provide) multiple different interactive
% setup routines that allow you to create a calibration file for your
% setup. Currently the following routines are provided:
%
% DisplayUndistortionBezier.m -- Undistortion based on a NURBS surface (Non
% uniform rational bezier surface).
%

% History:
% 19.7.2007 Written (MK).

global GL;

if isempty(GL)
    sca;
    error('PTB internal error: GL struct not initialized?!?');
end

if nargin < 2
    sca;
    error('PTB internal error: Must provide all parameters!');
end

% Load calibration file:
if ~exist(calibfilename, 'file')
    sca;
    error('In setup of geometry undistortion: No such calibration file %s!', calibfilename);
end

calib = load(calibfilename);

% Preinit warpstruct:
warpstruct.glsl = [];
warpstruct.gld = [];

% Type of setup depends on type of calibration:
switch(calib.warptype)
    case {'BezierDisplayList'}
        % Build warp display list for Bezier surface based
        % calibration/remapping:
        
        Screen('BeginOpenGL', window, 1);
        
        gld = glGenLists(1);
        
        % Build a display list that corresponds to the current calibration:
        glNewList(gld, GL.COMPILE);

        subdivision = calib.subdivision;
        
        % Setup a 2D parametric grid with 'subdivision' subdivisions:
        glMapGrid2d(subdivision, 0, 1, subdivision, 0, 1);

        % Enable Bezier evaluators:
        glEnable(GL.MAP2_VERTEX_3);
        glEnable(GL.MAP2_TEXTURE_COORD_2);

        % Setup initial mapping table for texture coordinates (source image control
        % points):
        frompts = calib.frompts;
        % Establish mapping for texture coordinates:
        glMap2d(GL.MAP2_TEXTURE_COORD_2, 0, 1, 2, size(frompts,2), 0, 1, 2*size(frompts,2), size(frompts,3), frompts);

        % Establish mapping for vertex coordinates:
        topts = calib.topts;
        % Setup mapping based on current control point matrix for destination
        % points:
        glMap2d(GL.MAP2_VERTEX_3, 0, 1, 3, size(topts,2), 0, 1, 3*size(topts,2), size(topts,3), topts);
        
        % Compute the mesh based on current mappings:
        glEvalMesh2(GL.FILL, 0, subdivision, 0, subdivision);
        
        % Disable mesh evaluators:
        glDisable(GL.MAP2_VERTEX_3);
        glDisable(GL.MAP2_TEXTURE_COORD_2);

        % List ready - and already updated in the imaging pipeline:
        glEndList;

        % Assign display list to output warpstruct:
        warpstruct.gld = gld;

        % Ready.
        Screen('EndOpenGL', window);
        
    otherwise
        sca;
        error('Unknown calibration method id: %s!', calib.warptype);
end

% Done. Return the warpstruct:
return;
