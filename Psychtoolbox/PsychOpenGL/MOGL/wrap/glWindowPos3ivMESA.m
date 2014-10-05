function glWindowPos3ivMESA( v )

% glWindowPos3ivMESA  Interface to OpenGL function glWindowPos3ivMESA
%
% usage:  glWindowPos3ivMESA( v )
%
% C function:  void glWindowPos3ivMESA(const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3ivMESA', int32(v) );

return
