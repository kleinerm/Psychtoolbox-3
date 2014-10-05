function glBindBufferBase( target, index, buffer )

% glBindBufferBase  Interface to OpenGL function glBindBufferBase
%
% usage:  glBindBufferBase( target, index, buffer )
%
% C function:  void glBindBufferBase(GLenum target, GLuint index, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferBase', target, index, buffer );

return
