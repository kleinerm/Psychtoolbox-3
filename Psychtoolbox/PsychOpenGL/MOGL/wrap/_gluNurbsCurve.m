function [ knots, control ] = gluNurbsCurve( nurb, knotCount, stride, order, type )

% gluNurbsCurve  Interface to OpenGL function gluNurbsCurve
%
% usage:  [ knots, control ] = gluNurbsCurve( nurb, knotCount, stride, order, type )
%
% C function:  void gluNurbsCurve(GLUnurbs* nurb, GLint knotCount, GLfloat* knots, GLint stride, GLfloat* control, GLint order, GLenum type)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

knots = single(0);
control = single(0);

moglcore( 'gluNurbsCurve', nurb, knotCount, knots, stride, control, order, type );

return
