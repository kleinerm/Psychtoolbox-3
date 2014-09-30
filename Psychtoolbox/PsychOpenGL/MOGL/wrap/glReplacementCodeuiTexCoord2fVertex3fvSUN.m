function glReplacementCodeuiTexCoord2fVertex3fvSUN( rc, tc, v )

% glReplacementCodeuiTexCoord2fVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiTexCoord2fVertex3fvSUN
%
% usage:  glReplacementCodeuiTexCoord2fVertex3fvSUN( rc, tc, v )
%
% C function:  void glReplacementCodeuiTexCoord2fVertex3fvSUN(const GLuint* rc, const GLfloat* tc, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiTexCoord2fVertex3fvSUN', uint32(rc), single(tc), single(v) );

return
