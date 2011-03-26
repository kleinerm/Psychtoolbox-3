function glClearIndex( c )

% glClearIndex  Interface to OpenGL function glClearIndex
%
% usage:  glClearIndex( c )
%
% C function:  void glClearIndex(GLfloat c)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glClearIndex', c );

return
