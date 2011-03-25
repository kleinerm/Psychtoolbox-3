function r = gluNewTess

% gluNewTess  Interface to OpenGL function gluNewTess
%
% usage:  r = gluNewTess
%
% C function:  GLUtesselator* gluNewTess(void)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

if nargin~=0,
    error('invalid number of arguments');
end

r = moglcore( 'gluNewTess' );

return
