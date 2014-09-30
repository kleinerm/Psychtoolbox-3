function glNormal3fVertex3fvSUN( n, v )

% glNormal3fVertex3fvSUN  Interface to OpenGL function glNormal3fVertex3fvSUN
%
% usage:  glNormal3fVertex3fvSUN( n, v )
%
% C function:  void glNormal3fVertex3fvSUN(const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNormal3fVertex3fvSUN', single(n), single(v) );

return
