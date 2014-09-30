function glFlushMappedNamedBufferRangeEXT( buffer, offset, length )

% glFlushMappedNamedBufferRangeEXT  Interface to OpenGL function glFlushMappedNamedBufferRangeEXT
%
% usage:  glFlushMappedNamedBufferRangeEXT( buffer, offset, length )
%
% C function:  void glFlushMappedNamedBufferRangeEXT(GLuint buffer, GLintptr offset, GLsizeiptr length)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFlushMappedNamedBufferRangeEXT', buffer, offset, length );

return
