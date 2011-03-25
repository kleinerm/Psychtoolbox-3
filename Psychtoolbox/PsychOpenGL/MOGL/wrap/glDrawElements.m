function glDrawElements( mode, count, type, indices )

% glDrawElements  Interface to OpenGL function glDrawElements
%
% usage:  glDrawElements( mode, count, type, indices )
%
% C function:  void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glDrawElements', mode, count, type, indices );

return
