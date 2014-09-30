function glColorSubTableEXT( target, start, count, format, type, data )

% glColorSubTableEXT  Interface to OpenGL function glColorSubTableEXT
%
% usage:  glColorSubTableEXT( target, start, count, format, type, data )
%
% C function:  void glColorSubTableEXT(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glColorSubTableEXT', target, start, count, format, type, data );

return
