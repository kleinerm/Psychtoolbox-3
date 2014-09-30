function glMultiDrawElementArrayAPPLE( mode, first, count, primcount )

% glMultiDrawElementArrayAPPLE  Interface to OpenGL function glMultiDrawElementArrayAPPLE
%
% usage:  glMultiDrawElementArrayAPPLE( mode, first, count, primcount )
%
% C function:  void glMultiDrawElementArrayAPPLE(GLenum mode, const GLint* first, const GLsizei* count, GLsizei primcount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementArrayAPPLE', mode, int32(first), int32(count), primcount );

return
