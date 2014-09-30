function glTexCoord4fColor4fNormal3fVertex4fvSUN( tc, c, n, v )

% glTexCoord4fColor4fNormal3fVertex4fvSUN  Interface to OpenGL function glTexCoord4fColor4fNormal3fVertex4fvSUN
%
% usage:  glTexCoord4fColor4fNormal3fVertex4fvSUN( tc, c, n, v )
%
% C function:  void glTexCoord4fColor4fNormal3fVertex4fvSUN(const GLfloat* tc, const GLfloat* c, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4fColor4fNormal3fVertex4fvSUN', single(tc), single(c), single(n), single(v) );

return
