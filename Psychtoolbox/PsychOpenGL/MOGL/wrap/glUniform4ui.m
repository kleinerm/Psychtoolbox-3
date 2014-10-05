function glUniform4ui( location, v0, v1, v2, v3 )

% glUniform4ui  Interface to OpenGL function glUniform4ui
%
% usage:  glUniform4ui( location, v0, v1, v2, v3 )
%
% C function:  void glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUniform4ui', location, v0, v1, v2, v3 );

return
