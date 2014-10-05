function glColor3fVertex3fvSUN( c, v )

% glColor3fVertex3fvSUN  Interface to OpenGL function glColor3fVertex3fvSUN
%
% usage:  glColor3fVertex3fvSUN( c, v )
%
% C function:  void glColor3fVertex3fvSUN(const GLfloat* c, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glColor3fVertex3fvSUN', single(c), single(v) );

return
