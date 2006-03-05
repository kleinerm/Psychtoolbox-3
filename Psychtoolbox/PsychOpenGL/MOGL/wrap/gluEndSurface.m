function gluEndSurface( nurb )

% gluEndSurface  Interface to OpenGL function gluEndSurface
%
% usage:  gluEndSurface( nurb )
%
% C function:  void gluEndSurface(GLUnurbs* nurb)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluEndSurface', nurb );

return
