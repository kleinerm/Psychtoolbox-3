function glPixelTransferi( pname, param )

% glPixelTransferi  Interface to OpenGL function glPixelTransferi
%
% usage:  glPixelTransferi( pname, param )
%
% C function:  void glPixelTransferi(GLenum pname, GLint param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPixelTransferi', pname, param );

return
