function glClientAttribDefaultEXT( mask )

% glClientAttribDefaultEXT  Interface to OpenGL function glClientAttribDefaultEXT
%
% usage:  glClientAttribDefaultEXT( mask )
%
% C function:  void glClientAttribDefaultEXT(GLbitfield mask)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClientAttribDefaultEXT', mask );

return
