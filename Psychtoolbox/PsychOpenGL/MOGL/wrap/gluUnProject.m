function [ objX, objY, objZ, r ] = gluUnProject( winX, winY, winZ, model, proj, view )

% gluUnProject  Interface to OpenGL function gluUnProject
%
% usage:  [ objX, objY, objZ, r ] = gluUnProject( winX, winY, winZ, model, proj, view )
%
% C function:  GLint gluUnProject(GLdouble winX, GLdouble winY, GLdouble winZ, const GLdouble* model, const GLdouble* proj, const GLint* view, GLdouble* objX, GLdouble* objY, GLdouble* objZ)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=6,
    error('invalid number of arguments');
end

objX = double(0);
objY = double(0);
objZ = double(0);
r = moglcore( 'gluUnProject', winX, winY, winZ, double(model), double(proj), int32(view), objX, objY, objZ );

return
% ---skip---
