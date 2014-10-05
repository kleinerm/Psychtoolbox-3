function glAsyncMarkerSGIX( marker )

% glAsyncMarkerSGIX  Interface to OpenGL function glAsyncMarkerSGIX
%
% usage:  glAsyncMarkerSGIX( marker )
%
% C function:  void glAsyncMarkerSGIX(GLuint marker)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glAsyncMarkerSGIX', marker );

return
