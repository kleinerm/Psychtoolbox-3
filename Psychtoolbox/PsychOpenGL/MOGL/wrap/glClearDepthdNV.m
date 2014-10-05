function glClearDepthdNV( depth )

% glClearDepthdNV  Interface to OpenGL function glClearDepthdNV
%
% usage:  glClearDepthdNV( depth )
%
% C function:  void glClearDepthdNV(GLdouble depth)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClearDepthdNV', depth );

return
