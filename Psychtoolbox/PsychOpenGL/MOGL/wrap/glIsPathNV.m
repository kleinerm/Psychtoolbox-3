function r = glIsPathNV( path )

% glIsPathNV  Interface to OpenGL function glIsPathNV
%
% usage:  r = glIsPathNV( path )
%
% C function:  GLboolean glIsPathNV(GLuint path)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsPathNV', path );

return
