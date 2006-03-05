function glVertexAttrib4Nuiv( index, v )

% glVertexAttrib4Nuiv  Interface to OpenGL function glVertexAttrib4Nuiv
%
% usage:  glVertexAttrib4Nuiv( index, v )
%
% C function:  void glVertexAttrib4Nuiv(GLuint index, const GLuint* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4Nuiv', index, uint32(v) );

return
