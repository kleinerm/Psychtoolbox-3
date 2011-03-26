function glVertexAttrib4Nbv( index, v )

% glVertexAttrib4Nbv  Interface to OpenGL function glVertexAttrib4Nbv
%
% usage:  glVertexAttrib4Nbv( index, v )
%
% C function:  void glVertexAttrib4Nbv(GLuint index, const GLbyte* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4Nbv', index, int8(v) );

return
