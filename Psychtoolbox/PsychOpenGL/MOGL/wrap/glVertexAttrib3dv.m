function glVertexAttrib3dv( index, v )

% glVertexAttrib3dv  Interface to OpenGL function glVertexAttrib3dv
%
% usage:  glVertexAttrib3dv( index, v )
%
% C function:  void glVertexAttrib3dv(GLuint index, const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3dv', index, double(v) );

return
