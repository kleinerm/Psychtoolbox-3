function glTexCoord2fColor3fVertex3fvSUN( tc, c, v )

% glTexCoord2fColor3fVertex3fvSUN  Interface to OpenGL function glTexCoord2fColor3fVertex3fvSUN
%
% usage:  glTexCoord2fColor3fVertex3fvSUN( tc, c, v )
%
% C function:  void glTexCoord2fColor3fVertex3fvSUN(const GLfloat* tc, const GLfloat* c, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fColor3fVertex3fvSUN', single(tc), single(c), single(v) );

return
