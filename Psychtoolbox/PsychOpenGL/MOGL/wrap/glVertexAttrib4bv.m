function glVertexAttrib4bv( index, v )

% glVertexAttrib4bv  Interface to OpenGL function glVertexAttrib4bv
%
% usage:  glVertexAttrib4bv( index, v )
%
% C function:  void glVertexAttrib4bv(GLuint index, const GLbyte* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4bv', index, int8(v) );

return
