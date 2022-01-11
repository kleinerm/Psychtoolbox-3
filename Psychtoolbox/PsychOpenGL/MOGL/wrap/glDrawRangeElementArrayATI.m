function glDrawRangeElementArrayATI( mode, startidx, endidx, count )

% glDrawRangeElementArrayATI  Interface to OpenGL function glDrawRangeElementArrayATI
%
% usage:  glDrawRangeElementArrayATI( mode, startidx, endidx, count )
%
% C function:  void glDrawRangeElementArrayATI(GLenum mode, GLuint start, GLuint end, GLsizei count)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glDrawRangeElementArrayATI', mode, startidx, endidx, count );

return
