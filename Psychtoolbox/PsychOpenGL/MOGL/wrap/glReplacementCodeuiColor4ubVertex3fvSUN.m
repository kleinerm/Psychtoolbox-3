function glReplacementCodeuiColor4ubVertex3fvSUN( rc, c, v )

% glReplacementCodeuiColor4ubVertex3fvSUN  Interface to OpenGL function glReplacementCodeuiColor4ubVertex3fvSUN
%
% usage:  glReplacementCodeuiColor4ubVertex3fvSUN( rc, c, v )
%
% C function:  void glReplacementCodeuiColor4ubVertex3fvSUN(const GLuint* rc, const GLubyte* c, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glReplacementCodeuiColor4ubVertex3fvSUN', uint32(rc), uint8(c), single(v) );

return
