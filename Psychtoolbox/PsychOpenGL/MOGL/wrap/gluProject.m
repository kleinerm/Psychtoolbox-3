function [ winX, winY, winZ, r ] = gluProject( objX, objY, objZ, model, proj, view )

% gluProject  Interface to OpenGL function gluProject
%
% usage:  [ winX, winY, winZ, r ] = gluProject( objX, objY, objZ, model, proj, view )
%
% C function:  GLint gluProject(GLdouble objX, GLdouble objY, GLdouble objZ, const GLdouble* model, const GLdouble* proj, const GLint* view, GLdouble* winX, GLdouble* winY, GLdouble* winZ)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=6,
    error('invalid number of arguments');
end

winX = double([0,0]);
winY = double([0,0]);
winZ = double([0,0]);
r = moglcore( 'gluProject', objX, objY, objZ, double(model), double(proj), int32(view), winX, winY, winZ );
winX = winX(1:end-1);
winY = winY(1:end-1);
winZ = winZ(1:end-1);
return
% ---skip---
