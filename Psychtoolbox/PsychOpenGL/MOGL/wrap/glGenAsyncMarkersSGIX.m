function r = glGenAsyncMarkersSGIX( range )

% glGenAsyncMarkersSGIX  Interface to OpenGL function glGenAsyncMarkersSGIX
%
% usage:  r = glGenAsyncMarkersSGIX( range )
%
% C function:  GLuint glGenAsyncMarkersSGIX(GLsizei range)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGenAsyncMarkersSGIX', range );

return
