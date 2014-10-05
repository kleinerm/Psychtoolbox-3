function glBindBufferOffsetNV( target, index, buffer, offset )

% glBindBufferOffsetNV  Interface to OpenGL function glBindBufferOffsetNV
%
% usage:  glBindBufferOffsetNV( target, index, buffer, offset )
%
% C function:  void glBindBufferOffsetNV(GLenum target, GLuint index, GLuint buffer, GLintptr offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferOffsetNV', target, index, buffer, offset );

return
