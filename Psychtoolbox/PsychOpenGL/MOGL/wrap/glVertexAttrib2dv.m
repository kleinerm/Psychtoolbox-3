function glVertexAttrib2dv( index, v )

% glVertexAttrib2dv  Interface to OpenGL function glVertexAttrib2dv
%
% usage:  glVertexAttrib2dv( index, v )
%
% C function:  void glVertexAttrib2dv(GLuint index, const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2dv', index, double(v) );

return
