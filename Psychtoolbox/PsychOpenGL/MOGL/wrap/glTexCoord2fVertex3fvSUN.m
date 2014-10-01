function glTexCoord2fVertex3fvSUN( tc, v )

% glTexCoord2fVertex3fvSUN  Interface to OpenGL function glTexCoord2fVertex3fvSUN
%
% usage:  glTexCoord2fVertex3fvSUN( tc, v )
%
% C function:  void glTexCoord2fVertex3fvSUN(const GLfloat* tc, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fVertex3fvSUN', single(tc), single(v) );

return
