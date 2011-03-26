function [ objX, objY, objZ, objW, r ] = gluUnProject4( winX, winY, winZ, clipW, model, proj, view, near, far )

% gluUnProject4  Interface to OpenGL function gluUnProject4
%
% usage:  [ objX, objY, objZ, objW, r ] = gluUnProject4( winX, winY, winZ, clipW, model, proj, view, near, far )
%
% C function:  GLint gluUnProject4(GLdouble winX, GLdouble winY, GLdouble winZ, GLdouble clipW, const GLdouble* model, const GLdouble* proj, const GLint* view, GLdouble near, GLdouble far, GLdouble* objX, GLdouble* objY, GLdouble* objZ, GLdouble* objW)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=9,
    error('invalid number of arguments');
end

objX = double(0);
objY = double(0);
objZ = double(0);
objW = double(0);
r = moglcore( 'gluUnProject4', winX, winY, winZ, clipW, double(model), double(proj), int32(view), near, far, objX, objY, objZ, objW );

return
% ---skip---
