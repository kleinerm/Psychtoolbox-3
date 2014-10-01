function glResetMinmaxEXT( target )

% glResetMinmaxEXT  Interface to OpenGL function glResetMinmaxEXT
%
% usage:  glResetMinmaxEXT( target )
%
% C function:  void glResetMinmaxEXT(GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glResetMinmaxEXT', target );

return
