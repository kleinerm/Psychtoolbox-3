function glMultiDrawRangeElementArrayAPPLE( mode, start, end, first, count, primcount )

% glMultiDrawRangeElementArrayAPPLE  Interface to OpenGL function glMultiDrawRangeElementArrayAPPLE
%
% usage:  glMultiDrawRangeElementArrayAPPLE( mode, start, end, first, count, primcount )
%
% C function:  void glMultiDrawRangeElementArrayAPPLE(GLenum mode, GLuint start, GLuint end, const GLint* first, const GLsizei* count, GLsizei primcount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawRangeElementArrayAPPLE', mode, start, end, int32(first), int32(count), primcount );

return
