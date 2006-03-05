function glVertexAttrib2dv( index, v )

% glVertexAttrib2dv  Interface to OpenGL function glVertexAttrib2dv
%
% usage:  glVertexAttrib2dv( index, v )
%
% C function:  void glVertexAttrib2dv(GLuint index, const GLdouble* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2dv', index, double(v) );

return
