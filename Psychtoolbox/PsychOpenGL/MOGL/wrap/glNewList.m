function glNewList( list, mode )

% glNewList  Interface to OpenGL function glNewList
%
% usage:  glNewList( list, mode )
%
% C function:  void glNewList(GLuint list, GLenum mode)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glNewList', list, mode );

return
