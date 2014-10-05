function glReplacementCodeuiVertex3fvSUN( rc, v )

% glReplacementCodeuiVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiVertex3fvSUN
%
% usage:  glReplacementCodeuiVertex3fvSUN( rc, v )
%
% C function:  void glReplacementCodeuiVertex3fvSUN(const GLuint* rc, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiVertex3fvSUN', uint32(rc), single(v) );

return
