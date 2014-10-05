function glBindBufferBaseEXT( target, index, buffer )

% glBindBufferBaseEXT  Interface to OpenGL function glBindBufferBaseEXT
%
% usage:  glBindBufferBaseEXT( target, index, buffer )
%
% C function:  void glBindBufferBaseEXT(GLenum target, GLuint index, GLuint buffer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferBaseEXT', target, index, buffer );

return
