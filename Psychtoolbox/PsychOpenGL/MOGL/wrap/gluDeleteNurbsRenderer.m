function gluDeleteNurbsRenderer( nurb )

% gluDeleteNurbsRenderer  Interface to OpenGL function gluDeleteNurbsRenderer
%
% usage:  gluDeleteNurbsRenderer( nurb )
%
% C function:  void gluDeleteNurbsRenderer(GLUnurbs* nurb)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'double'),
	error([ 'argument ''nurb'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluDeleteNurbsRenderer', nurb );

return
