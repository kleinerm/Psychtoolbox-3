function glTexCoord2fNormal3fVertex3fvSUN( tc, n, v )

% glTexCoord2fNormal3fVertex3fvSUN  Interface to OpenGL function glTexCoord2fNormal3fVertex3fvSUN
%
% usage:  glTexCoord2fNormal3fVertex3fvSUN( tc, n, v )
%
% C function:  void glTexCoord2fNormal3fVertex3fvSUN(const GLfloat* tc, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fNormal3fVertex3fvSUN', single(tc), single(n), single(v) );

return
