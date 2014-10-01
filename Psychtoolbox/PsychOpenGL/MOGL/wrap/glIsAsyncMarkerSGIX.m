function r = glIsAsyncMarkerSGIX( marker )

% glIsAsyncMarkerSGIX  Interface to OpenGL function glIsAsyncMarkerSGIX
%
% usage:  r = glIsAsyncMarkerSGIX( marker )
%
% C function:  GLboolean glIsAsyncMarkerSGIX(GLuint marker)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsAsyncMarkerSGIX', marker );

return
