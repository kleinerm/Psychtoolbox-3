function glMultiDrawElementsEXT( mode, count, type, const, primcount )

% glMultiDrawElementsEXT  Interface to OpenGL function glMultiDrawElementsEXT
%
% usage:  glMultiDrawElementsEXT( mode, count, type, const, primcount )
%
% C function:  void glMultiDrawElementsEXT(GLenum mode, const GLsizei* count, GLenum type, const void* const, GLsizei primcount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementsEXT', mode, int32(count), type, const, primcount );

return
