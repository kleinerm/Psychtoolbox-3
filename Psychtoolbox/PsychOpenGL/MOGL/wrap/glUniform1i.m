function glUniform1i( location, v0 )

% glUniform1i  Interface to OpenGL function glUniform1i
%
% usage:  glUniform1i( location, v0 )
%
% C function:  void glUniform1i(GLint location, GLint v0)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glUniform1i', location, v0 );

return
