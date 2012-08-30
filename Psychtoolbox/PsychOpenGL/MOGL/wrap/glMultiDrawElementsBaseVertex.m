function glMultiDrawElementsBaseVertex( mode, count, type, const, drawcount, basevertex )

% glMultiDrawElementsBaseVertex  Interface to OpenGL function glMultiDrawElementsBaseVertex
%
% usage:  glMultiDrawElementsBaseVertex( mode, count, type, const, drawcount, basevertex )
%
% C function:  void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei* count, GLenum type, const GLvoid* const, GLsizei drawcount, const GLint* basevertex)

% 30-Aug-2012 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementsBaseVertex', mode, int32(count), type, const, drawcount, int32(basevertex) );

return
