function r = glRenderMode( mode )

% glRenderMode  Interface to OpenGL function glRenderMode
%
% usage:  r = glRenderMode( mode )
%
% C function:  GLint glRenderMode(GLenum mode)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glRenderMode', mode );

return
