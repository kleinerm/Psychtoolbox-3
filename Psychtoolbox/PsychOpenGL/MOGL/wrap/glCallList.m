function glCallList( list )

% glCallList  Interface to OpenGL function glCallList
%
% usage:  glCallList( list )
%
% C function:  void glCallList(GLuint list)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glCallList', list );

return
