function glDeleteAsyncMarkersSGIX( marker, range )

% glDeleteAsyncMarkersSGIX  Interface to OpenGL function glDeleteAsyncMarkersSGIX
%
% usage:  glDeleteAsyncMarkersSGIX( marker, range )
%
% C function:  void glDeleteAsyncMarkersSGIX(GLuint marker, GLsizei range)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteAsyncMarkersSGIX', marker, range );

return
