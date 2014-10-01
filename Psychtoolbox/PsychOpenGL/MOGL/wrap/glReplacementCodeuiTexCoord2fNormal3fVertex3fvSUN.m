function glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN( rc, tc, n, v )

% glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN
%
% usage:  glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN( rc, tc, n, v )
%
% C function:  void glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN(const GLuint* rc, const GLfloat* tc, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiTexCoord2fNormal3fVertex3fvSUN', uint32(rc), single(tc), single(n), single(v) );

return
