function glPixelStorei( pname, param )

% glPixelStorei  Interface to OpenGL function glPixelStorei
%
% usage:  glPixelStorei( pname, param )
%
% C function:  void glPixelStorei(GLenum pname, GLint param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPixelStorei', pname, param );

return
