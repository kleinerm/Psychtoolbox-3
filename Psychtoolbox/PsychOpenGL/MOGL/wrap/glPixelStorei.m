function glPixelStorei( pname, param )

% glPixelStorei  Interface to OpenGL function glPixelStorei
%
% usage:  glPixelStorei( pname, param )
%
% C function:  void glPixelStorei(GLenum pname, GLint param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPixelStorei', pname, param );

return
