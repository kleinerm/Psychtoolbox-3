function glFlushMappedBufferRange( target, ptr1, ptr2 )

% glFlushMappedBufferRange  Interface to OpenGL function glFlushMappedBufferRange
%
% usage:  glFlushMappedBufferRange( target, ptr1, ptr2 )
%
% C function:  void glFlushMappedBufferRange(GLenum target, GLint ptr, GLsizei ptr)

% 30-Aug-2012 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFlushMappedBufferRange', target, ptr1, ptr2 );

return
