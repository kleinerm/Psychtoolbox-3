function glEnableClientStateiEXT( array, index )

% glEnableClientStateiEXT  Interface to OpenGL function glEnableClientStateiEXT
%
% usage:  glEnableClientStateiEXT( array, index )
%
% C function:  void glEnableClientStateiEXT(GLenum array, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glEnableClientStateiEXT', array, index );

return
