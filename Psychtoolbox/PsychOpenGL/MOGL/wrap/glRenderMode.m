function r = glRenderMode( mode )

% glRenderMode  Interface to OpenGL function glRenderMode
%
% usage:  r = glRenderMode( mode )
%
% C function:  GLint glRenderMode(GLenum mode)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glRenderMode', mode );

return
