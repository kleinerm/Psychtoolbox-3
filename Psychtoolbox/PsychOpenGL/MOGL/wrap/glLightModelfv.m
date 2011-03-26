function glLightModelfv( pname, params )

% glLightModelfv  Interface to OpenGL function glLightModelfv
%
% usage:  glLightModelfv( pname, params )
%
% C function:  void glLightModelfv(GLenum pname, const GLfloat* params)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glLightModelfv', pname, single(params) );

return
