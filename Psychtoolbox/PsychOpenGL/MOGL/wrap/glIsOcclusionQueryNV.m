function r = glIsOcclusionQueryNV( id )

% glIsOcclusionQueryNV  Interface to OpenGL function glIsOcclusionQueryNV
%
% usage:  r = glIsOcclusionQueryNV( id )
%
% C function:  GLboolean glIsOcclusionQueryNV(GLuint id)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsOcclusionQueryNV', id );

return
