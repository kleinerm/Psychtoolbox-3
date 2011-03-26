function gluLoadSamplingMatrices( nurb, model, perspective, view )

% gluLoadSamplingMatrices  Interface to OpenGL function gluLoadSamplingMatrices
%
% usage:  gluLoadSamplingMatrices( nurb, model, perspective, view )
%
% C function:  void gluLoadSamplingMatrices(GLUnurbs* nurb, const GLfloat* model, const GLfloat* perspective, const GLint* view)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'double'),
	error([ 'argument ''nurb'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluLoadSamplingMatrices', nurb, single(model), single(perspective), int32(view) );

return
