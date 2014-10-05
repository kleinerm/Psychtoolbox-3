function glReplacementCodeuiColor4fNormal3fVertex3fvSUN( rc, c, n, v )

% glReplacementCodeuiColor4fNormal3fVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiColor4fNormal3fVertex3fvSUN
%
% usage:  glReplacementCodeuiColor4fNormal3fVertex3fvSUN( rc, c, n, v )
%
% C function:  void glReplacementCodeuiColor4fNormal3fVertex3fvSUN(const GLuint* rc, const GLfloat* c, const GLfloat* n, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiColor4fNormal3fVertex3fvSUN', uint32(rc), single(c), single(n), single(v) );

return
