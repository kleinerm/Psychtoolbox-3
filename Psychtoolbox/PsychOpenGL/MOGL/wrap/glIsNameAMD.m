function r = glIsNameAMD( identifier, name )

% glIsNameAMD  Interface to OpenGL function glIsNameAMD
%
% usage:  r = glIsNameAMD( identifier, name )
%
% C function:  GLboolean glIsNameAMD(GLenum identifier, GLuint name)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glIsNameAMD', identifier, name );

return
