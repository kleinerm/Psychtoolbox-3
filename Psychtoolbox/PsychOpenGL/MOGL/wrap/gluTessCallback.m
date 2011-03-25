function gluTessCallback( tess, which, callback )

% gluTessCallback  Interface to OpenGL function gluTessCallback
%
% usage:  gluTessCallback( tess, which, callback )
%
% C function:  void gluTessCallback(GLUtesselator* tess, GLenum which, GLvoid* callback)

% 11-Apr-2010 -- created manually
% 24-Mar-2011 -- Made 64-bit clean (MK)

% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'double'),
	error([ 'argument ''tess'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluTessCallback', tess, which, callback );

return
