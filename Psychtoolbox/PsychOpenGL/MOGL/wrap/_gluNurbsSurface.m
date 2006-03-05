function [ sKnots, tKnots, control ] = gluNurbsSurface( nurb, sKnotCount, tKnotCount, sStride, tStride, sOrder, tOrder, type )

% gluNurbsSurface  Interface to OpenGL function gluNurbsSurface
%
% usage:  [ sKnots, tKnots, control ] = gluNurbsSurface( nurb, sKnotCount, tKnotCount, sStride, tStride, sOrder, tOrder, type )
%
% C function:  void gluNurbsSurface(GLUnurbs* nurb, GLint sKnotCount, GLfloat* sKnots, GLint tKnotCount, GLfloat* tKnots, GLint sStride, GLint tStride, GLfloat* control, GLint sOrder, GLint tOrder, GLenum type)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=8,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

sKnots = single(0);
tKnots = single(0);
control = single(0);

moglcore( 'gluNurbsSurface', nurb, sKnotCount, sKnots, tKnotCount, tKnots, sStride, tStride, control, sOrder, tOrder, type );

return
