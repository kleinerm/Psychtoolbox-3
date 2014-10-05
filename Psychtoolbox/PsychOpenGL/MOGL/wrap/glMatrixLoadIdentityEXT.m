function glMatrixLoadIdentityEXT( mode )

% glMatrixLoadIdentityEXT  Interface to OpenGL function glMatrixLoadIdentityEXT
%
% usage:  glMatrixLoadIdentityEXT( mode )
%
% C function:  void glMatrixLoadIdentityEXT(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoadIdentityEXT', mode );

return
