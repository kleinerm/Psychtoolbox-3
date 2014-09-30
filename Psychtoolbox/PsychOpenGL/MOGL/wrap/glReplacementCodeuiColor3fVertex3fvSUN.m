function glReplacementCodeuiColor3fVertex3fvSUN( rc, c, v )

% glReplacementCodeuiColor3fVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiColor3fVertex3fvSUN
%
% usage:  glReplacementCodeuiColor3fVertex3fvSUN( rc, c, v )
%
% C function:  void glReplacementCodeuiColor3fVertex3fvSUN(const GLuint* rc, const GLfloat* c, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiColor3fVertex3fvSUN', uint32(rc), single(c), single(v) );

return
