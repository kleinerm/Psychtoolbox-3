function glMatrixPushEXT( mode )

% glMatrixPushEXT  Interface to OpenGL function glMatrixPushEXT
%
% usage:  glMatrixPushEXT( mode )
%
% C function:  void glMatrixPushEXT(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMatrixPushEXT', mode );

return
