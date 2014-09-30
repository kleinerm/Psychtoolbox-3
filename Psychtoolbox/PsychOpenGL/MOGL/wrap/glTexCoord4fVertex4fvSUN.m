function glTexCoord4fVertex4fvSUN( tc, v )

% glTexCoord4fVertex4fvSUN  Interface to OpenGL function glTexCoord4fVertex4fvSUN
%
% usage:  glTexCoord4fVertex4fvSUN( tc, v )
%
% C function:  void glTexCoord4fVertex4fvSUN(const GLfloat* tc, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord4fVertex4fvSUN', single(tc), single(v) );

return
