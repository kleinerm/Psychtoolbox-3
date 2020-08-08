function glFramebufferSampleLocationsfvNV( target, start, count, v )

% glFramebufferSampleLocationsfvNV  Interface to OpenGL function glFramebufferSampleLocationsfvNV
%
% usage:  glFramebufferSampleLocationsfvNV( target, start, count, v )
%
% C function:  void glFramebufferSampleLocationsfvNV(GLenum target, GLuint start, GLsizei count, const GLfloat* v)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glFramebufferSampleLocationsfvNV', target, start, count, single(v) );

return
