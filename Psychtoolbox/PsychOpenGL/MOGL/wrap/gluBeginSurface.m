function gluBeginSurface( nurb )

% gluBeginSurface  Interface to OpenGL function gluBeginSurface
%
% usage:  gluBeginSurface( nurb )
%
% C function:  void gluBeginSurface(GLUnurbs* nurb)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'double'),
	error([ 'argument ''nurb'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluBeginSurface', nurb );

return
