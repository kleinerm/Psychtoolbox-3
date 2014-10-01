function glBindBufferOffsetEXT( target, index, buffer, offset )

% glBindBufferOffsetEXT  Interface to OpenGL function glBindBufferOffsetEXT
%
% usage:  glBindBufferOffsetEXT( target, index, buffer, offset )
%
% C function:  void glBindBufferOffsetEXT(GLenum target, GLuint index, GLuint buffer, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferOffsetEXT', target, index, buffer, offset );

return
