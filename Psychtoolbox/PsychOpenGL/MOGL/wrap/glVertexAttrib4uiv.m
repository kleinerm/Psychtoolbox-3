function glVertexAttrib4uiv( index, v )

% glVertexAttrib4uiv  Interface to OpenGL function glVertexAttrib4uiv
%
% usage:  glVertexAttrib4uiv( index, v )
%
% C function:  void glVertexAttrib4uiv(GLuint index, const GLuint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4uiv', index, uint32(v) );

return
