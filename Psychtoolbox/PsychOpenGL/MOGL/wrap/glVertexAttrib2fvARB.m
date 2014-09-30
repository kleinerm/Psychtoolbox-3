function glVertexAttrib2fvARB( index, v )

% glVertexAttrib2fvARB  Interface to OpenGL function glVertexAttrib2fvARB
%
% usage:  glVertexAttrib2fvARB( index, v )
%
% C function:  void glVertexAttrib2fvARB(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2fvARB', index, single(v) );

return
