function glElementPointerATI( type, pointer )

% glElementPointerATI  Interface to OpenGL function glElementPointerATI
%
% usage:  glElementPointerATI( type, pointer )
%
% C function:  void glElementPointerATI(GLenum type, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glElementPointerATI', type, pointer );

return
