function gluNurbsCurve( nurb, knotCount, knots, stride, control, order, type )

% gluNurbsCurve  Interface to OpenGL function gluNurbsCurve
%
% usage:  gluNurbsCurve( nurb, knotCount, knots, stride, control, order, type )
%
% C function:  void gluNurbsCurve(GLUnurbs* nurb, GLint knotCount, GLfloat* knots, GLint stride, GLfloat* control, GLint order, GLenum type)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=5,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluNurbsCurve', nurb, knotCount, moglsingle(knots), stride, moglsingle(control), order, type );

return
