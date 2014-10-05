function glMultiDrawArraysEXT( mode, first, count, primcount )

% glMultiDrawArraysEXT  Interface to OpenGL function glMultiDrawArraysEXT
%
% usage:  glMultiDrawArraysEXT( mode, first, count, primcount )
%
% C function:  void glMultiDrawArraysEXT(GLenum mode, const GLint* first, const GLsizei* count, GLsizei primcount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawArraysEXT', mode, int32(first), int32(count), primcount );

return
