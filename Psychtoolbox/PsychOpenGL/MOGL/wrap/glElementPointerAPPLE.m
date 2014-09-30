function glElementPointerAPPLE( type, pointer )

% glElementPointerAPPLE  Interface to OpenGL function glElementPointerAPPLE
%
% usage:  glElementPointerAPPLE( type, pointer )
%
% C function:  void glElementPointerAPPLE(GLenum type, const void* pointer)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glElementPointerAPPLE', type, pointer );

return
