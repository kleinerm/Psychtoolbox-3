function glVertexAttrib1dv( index, v )

% glVertexAttrib1dv  Interface to OpenGL function glVertexAttrib1dv
%
% usage:  glVertexAttrib1dv( index, v )
%
% C function:  void glVertexAttrib1dv(GLuint index, const GLdouble* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1dv', index, double(v) );

return
