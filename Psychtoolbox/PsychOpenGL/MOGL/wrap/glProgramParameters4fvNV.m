function glProgramParameters4fvNV( target, index, count, v )

% glProgramParameters4fvNV  Interface to OpenGL function glProgramParameters4fvNV
%
% usage:  glProgramParameters4fvNV( target, index, count, v )
%
% C function:  void glProgramParameters4fvNV(GLenum target, GLuint index, GLsizei count, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramParameters4fvNV', target, index, count, single(v) );

return
