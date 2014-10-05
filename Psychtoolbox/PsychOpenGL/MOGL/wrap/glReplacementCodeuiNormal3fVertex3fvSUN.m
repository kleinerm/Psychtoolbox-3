function glReplacementCodeuiNormal3fVertex3fvSUN( rc, n, v )

% glReplacementCodeuiNormal3fVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiNormal3fVertex3fvSUN
%
% usage:  glReplacementCodeuiNormal3fVertex3fvSUN( rc, n, v )
%
% C function:  void glReplacementCodeuiNormal3fVertex3fvSUN(const GLuint* rc, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiNormal3fVertex3fvSUN', uint32(rc), single(n), single(v) );

return
