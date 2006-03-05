function gluEndCurve( nurb )

% gluEndCurve  Interface to OpenGL function gluEndCurve
%
% usage:  gluEndCurve( nurb )
%
% C function:  void gluEndCurve(GLUnurbs* nurb)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluEndCurve', nurb );

return
