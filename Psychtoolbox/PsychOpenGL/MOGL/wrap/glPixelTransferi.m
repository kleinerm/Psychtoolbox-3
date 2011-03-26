function glPixelTransferi( pname, param )

% glPixelTransferi  Interface to OpenGL function glPixelTransferi
%
% usage:  glPixelTransferi( pname, param )
%
% C function:  void glPixelTransferi(GLenum pname, GLint param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPixelTransferi', pname, param );

return
