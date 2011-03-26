function glUniform3i( location, v0, v1, v2 )

% glUniform3i  Interface to OpenGL function glUniform3i
%
% usage:  glUniform3i( location, v0, v1, v2 )
%
% C function:  void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glUniform3i', location, v0, v1, v2 );

return
