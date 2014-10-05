function glDrawElementArrayATI( mode, count )

% glDrawElementArrayATI  Interface to OpenGL function glDrawElementArrayATI
%
% usage:  glDrawElementArrayATI( mode, count )
%
% C function:  void glDrawElementArrayATI(GLenum mode, GLsizei count)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDrawElementArrayATI', mode, count );

return
