function glFlushMappedBufferRangeAPPLE( target, offset, size )

% glFlushMappedBufferRangeAPPLE  Interface to OpenGL function glFlushMappedBufferRangeAPPLE
%
% usage:  glFlushMappedBufferRangeAPPLE( target, offset, size )
%
% C function:  void glFlushMappedBufferRangeAPPLE(GLenum target, GLintptr offset, GLsizeiptr size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFlushMappedBufferRangeAPPLE', target, offset, size );

return
