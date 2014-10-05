function glProgramParameters4dvNV( target, index, count, v )

% glProgramParameters4dvNV  Interface to OpenGL function glProgramParameters4dvNV
%
% usage:  glProgramParameters4dvNV( target, index, count, v )
%
% C function:  void glProgramParameters4dvNV(GLenum target, GLuint index, GLsizei count, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glProgramParameters4dvNV', target, index, count, double(v) );

return
