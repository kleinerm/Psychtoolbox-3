function glDrawRangeElementArrayAPPLE( mode, start, end, first, count )

% glDrawRangeElementArrayAPPLE  Interface to OpenGL function glDrawRangeElementArrayAPPLE
%
% usage:  glDrawRangeElementArrayAPPLE( mode, start, end, first, count )
%
% C function:  void glDrawRangeElementArrayAPPLE(GLenum mode, GLuint start, GLuint end, GLint first, GLsizei count)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glDrawRangeElementArrayAPPLE', mode, start, end, first, count );

return
