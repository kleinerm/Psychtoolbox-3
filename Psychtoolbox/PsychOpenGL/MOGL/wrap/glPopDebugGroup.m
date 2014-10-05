function glPopDebugGroup

% glPopDebugGroup  Interface to OpenGL function glPopDebugGroup
%
% usage:  glPopDebugGroup
%
% C function:  void glPopDebugGroup(void)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

moglcore( 'glPopDebugGroup' );

return
