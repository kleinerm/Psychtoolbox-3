function glWindowPos4fvMESA( v )

% glWindowPos4fvMESA  Interface to OpenGL function glWindowPos4fvMESA
%
% usage:  glWindowPos4fvMESA( v )
%
% C function:  void glWindowPos4fvMESA(const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4fvMESA', single(v) );

return
