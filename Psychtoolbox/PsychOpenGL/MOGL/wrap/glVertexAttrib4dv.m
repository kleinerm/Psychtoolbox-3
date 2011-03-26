function glVertexAttrib4dv( index, v )

% glVertexAttrib4dv  Interface to OpenGL function glVertexAttrib4dv
%
% usage:  glVertexAttrib4dv( index, v )
%
% C function:  void glVertexAttrib4dv(GLuint index, const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4dv', index, double(v) );

return
