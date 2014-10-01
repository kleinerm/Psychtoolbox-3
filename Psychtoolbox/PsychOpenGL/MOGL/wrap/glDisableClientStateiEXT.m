function glDisableClientStateiEXT( array, index )

% glDisableClientStateiEXT  Interface to OpenGL function glDisableClientStateiEXT
%
% usage:  glDisableClientStateiEXT( array, index )
%
% C function:  void glDisableClientStateiEXT(GLenum array, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDisableClientStateiEXT', array, index );

return
