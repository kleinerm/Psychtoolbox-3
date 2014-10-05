function glUniform2ui( location, v0, v1 )

% glUniform2ui  Interface to OpenGL function glUniform2ui
%
% usage:  glUniform2ui( location, v0, v1 )
%
% C function:  void glUniform2ui(GLint location, GLuint v0, GLuint v1)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glUniform2ui', location, v0, v1 );

return
