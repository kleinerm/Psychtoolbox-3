function glTexCoord2fColor4fNormal3fVertex3fvSUN( tc, c, n, v )

% glTexCoord2fColor4fNormal3fVertex3fvSUN  Interface to OpenGL function glTexCoord2fColor4fNormal3fVertex3fvSUN
%
% usage:  glTexCoord2fColor4fNormal3fVertex3fvSUN( tc, c, n, v )
%
% C function:  void glTexCoord2fColor4fNormal3fVertex3fvSUN(const GLfloat* tc, const GLfloat* c, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fColor4fNormal3fVertex3fvSUN', single(tc), single(c), single(n), single(v) );

return
