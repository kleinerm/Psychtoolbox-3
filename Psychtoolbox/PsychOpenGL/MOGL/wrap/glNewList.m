function glNewList( list, mode )

% glNewList  Interface to OpenGL function glNewList
%
% usage:  glNewList( list, mode )
%
% C function:  void glNewList(GLuint list, GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNewList', list, mode );

return
