function glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN( rc, tc, c, n, v )

% glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN
%
% usage:  glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN( rc, tc, c, n, v )
%
% C function:  void glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN(const GLuint* rc, const GLfloat* tc, const GLfloat* c, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiTexCoord2fColor4fNormal3fVertex3fvSUN', uint32(rc), single(tc), single(c), single(n), single(v) );

return
