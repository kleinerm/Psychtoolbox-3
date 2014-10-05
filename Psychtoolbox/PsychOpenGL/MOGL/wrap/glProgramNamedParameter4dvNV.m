function glProgramNamedParameter4dvNV( id, len, name, v )

% glProgramNamedParameter4dvNV  Interface to OpenGL function glProgramNamedParameter4dvNV
%
% usage:  glProgramNamedParameter4dvNV( id, len, name, v )
%
% C function:  void glProgramNamedParameter4dvNV(GLuint id, GLsizei len, const GLubyte* name, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramNamedParameter4dvNV', id, len, uint8(name), double(v) );

return
