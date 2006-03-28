function glDrawRangeElements( mode, start, endidx, count, type, indices )

% glDrawRangeElements  Interface to OpenGL function glDrawRangeElements
%
% usage:  glDrawRangeElements( mode, start, endidx, count, type, indices )
%
% C function:  void glDrawRangeElements(GLenum mode, GLuint start, GLuint endidx, GLsizei count, GLenum type, const GLvoid* indices)

% 05-Mar-2006 -- created (generated automatically from header files, modified by MK)

% ---protected---

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glDrawRangeElements', mode, start, endidx, count, type, indices );

return
