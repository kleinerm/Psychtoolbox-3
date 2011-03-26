function glUniform4i( location, v0, v1, v2, v3 )

% glUniform4i  Interface to OpenGL function glUniform4i
%
% usage:  glUniform4i( location, v0, v1, v2, v3 )
%
% C function:  void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4i', location, v0, v1, v2, v3 );

return
