function glFrameZoomSGIX( factor )

% glFrameZoomSGIX  Interface to OpenGL function glFrameZoomSGIX
%
% usage:  glFrameZoomSGIX( factor )
%
% C function:  void glFrameZoomSGIX(GLint factor)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFrameZoomSGIX', factor );

return
