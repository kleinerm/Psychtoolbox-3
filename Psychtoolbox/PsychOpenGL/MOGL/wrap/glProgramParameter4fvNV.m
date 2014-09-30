function glProgramParameter4fvNV( target, index, v )

% glProgramParameter4fvNV  Interface to OpenGL function glProgramParameter4fvNV
%
% usage:  glProgramParameter4fvNV( target, index, v )
%
% C function:  void glProgramParameter4fvNV(GLenum target, GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glProgramParameter4fvNV', target, index, single(v) );

return
