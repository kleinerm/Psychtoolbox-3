function glDeleteLists( list, range )

% glDeleteLists  Interface to OpenGL function glDeleteLists
%
% usage:  glDeleteLists( list, range )
%
% C function:  void glDeleteLists(GLuint list, GLsizei range)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteLists', list, range );

return
