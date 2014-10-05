function glColor4fNormal3fVertex3fvSUN( c, n, v )

% glColor4fNormal3fVertex3fvSUN  Interface to OpenGL function glColor4fNormal3fVertex3fvSUN
%
% usage:  glColor4fNormal3fVertex3fvSUN( c, n, v )
%
% C function:  void glColor4fNormal3fVertex3fvSUN(const GLfloat* c, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glColor4fNormal3fVertex3fvSUN', single(c), single(n), single(v) );

return
