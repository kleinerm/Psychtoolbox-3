function glPushName( name )

% glPushName  Interface to OpenGL function glPushName
%
% usage:  glPushName( name )
%
% C function:  void glPushName(GLuint name)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glPushName', name );

return
