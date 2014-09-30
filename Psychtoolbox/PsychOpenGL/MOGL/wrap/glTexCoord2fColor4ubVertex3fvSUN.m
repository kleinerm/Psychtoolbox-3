function glTexCoord2fColor4ubVertex3fvSUN( tc, c, v )

% glTexCoord2fColor4ubVertex3fvSUN  Interface to OpenGL function glTexCoord2fColor4ubVertex3fvSUN
%
% usage:  glTexCoord2fColor4ubVertex3fvSUN( tc, c, v )
%
% C function:  void glTexCoord2fColor4ubVertex3fvSUN(const GLfloat* tc, const GLubyte* c, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fColor4ubVertex3fvSUN', single(tc), uint8(c), single(v) );

return
