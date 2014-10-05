function glBindBuffersBase( target, first, count, buffers )

% glBindBuffersBase  Interface to OpenGL function glBindBuffersBase
%
% usage:  glBindBuffersBase( target, first, count, buffers )
%
% C function:  void glBindBuffersBase(GLenum target, GLuint first, GLsizei count, const GLuint* buffers)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBindBuffersBase', target, first, count, uint32(buffers) );

return
