function r = gluNewNurbsRenderer

% gluNewNurbsRenderer  Interface to OpenGL function gluNewNurbsRenderer
%
% usage:  r = gluNewNurbsRenderer
%
% C function:  GLUnurbs* gluNewNurbsRenderer(void)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moglcore( 'gluNewNurbsRenderer' );

return
