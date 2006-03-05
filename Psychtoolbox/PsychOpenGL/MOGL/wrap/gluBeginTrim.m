function gluBeginTrim( nurb )

% gluBeginTrim  Interface to OpenGL function gluBeginTrim
%
% usage:  gluBeginTrim( nurb )
%
% C function:  void gluBeginTrim(GLUnurbs* nurb)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluBeginTrim', nurb );

return
