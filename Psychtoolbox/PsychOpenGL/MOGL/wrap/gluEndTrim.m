function gluEndTrim( nurb )

% gluEndTrim  Interface to OpenGL function gluEndTrim
%
% usage:  gluEndTrim( nurb )
%
% C function:  void gluEndTrim(GLUnurbs* nurb)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluEndTrim', nurb );

return
