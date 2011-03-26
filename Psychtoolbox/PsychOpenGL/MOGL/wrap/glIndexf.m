function glIndexf( c )

% glIndexf  Interface to OpenGL function glIndexf
%
% usage:  glIndexf( c )
%
% C function:  void glIndexf(GLfloat c)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glIndexf', c );

return
