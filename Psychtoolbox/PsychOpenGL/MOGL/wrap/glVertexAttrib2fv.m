function glVertexAttrib2fv( index, v )

% glVertexAttrib2fv  Interface to OpenGL function glVertexAttrib2fv
%
% usage:  glVertexAttrib2fv( index, v )
%
% C function:  void glVertexAttrib2fv(GLuint index, const GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2fv', index, moglsingle(v) );

return
