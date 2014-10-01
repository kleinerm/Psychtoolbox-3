function glWindowPos2ivMESA( v )

% glWindowPos2ivMESA  Interface to OpenGL function glWindowPos2ivMESA
%
% usage:  glWindowPos2ivMESA( v )
%
% C function:  void glWindowPos2ivMESA(const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2ivMESA', int32(v) );

return
