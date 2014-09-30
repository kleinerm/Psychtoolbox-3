function glDrawElementArrayAPPLE( mode, first, count )

% glDrawElementArrayAPPLE  Interface to OpenGL function glDrawElementArrayAPPLE
%
% usage:  glDrawElementArrayAPPLE( mode, first, count )
%
% C function:  void glDrawElementArrayAPPLE(GLenum mode, GLint first, GLsizei count)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glDrawElementArrayAPPLE', mode, first, count );

return
