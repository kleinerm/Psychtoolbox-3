function glDrawRangeElements( mode, start, end, count, type, indices )

% glDrawRangeElements  Interface to OpenGL function glDrawRangeElements
%
% usage:  glDrawRangeElements( mode, start, end, count, type, indices )
%
% C function:  void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glDrawRangeElements', mode, start, end, count, type, indices );

return
