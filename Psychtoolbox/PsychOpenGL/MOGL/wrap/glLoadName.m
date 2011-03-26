function glLoadName( name )

% glLoadName  Interface to OpenGL function glLoadName
%
% usage:  glLoadName( name )
%
% C function:  void glLoadName(GLuint name)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadName', name );

return
