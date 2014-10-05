function glProgramNamedParameter4fvNV( id, len, name, v )

% glProgramNamedParameter4fvNV  Interface to OpenGL function glProgramNamedParameter4fvNV
%
% usage:  glProgramNamedParameter4fvNV( id, len, name, v )
%
% C function:  void glProgramNamedParameter4fvNV(GLuint id, GLsizei len, const GLubyte* name, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramNamedParameter4fvNV', id, len, uint8(name), single(v) );

return
