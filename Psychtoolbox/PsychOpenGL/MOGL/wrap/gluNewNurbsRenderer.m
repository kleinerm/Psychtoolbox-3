function r = gluNewNurbsRenderer

% gluNewNurbsRenderer  Interface to OpenGL function gluNewNurbsRenderer
%
% usage:  r = gluNewNurbsRenderer
%
% C function:  GLUnurbs* gluNewNurbsRenderer(void)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moglcore( 'gluNewNurbsRenderer' );

return
