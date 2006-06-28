function gluLoadSamplingMatrices( nurb, model, perspective, view )

% gluLoadSamplingMatrices  Interface to OpenGL function gluLoadSamplingMatrices
%
% usage:  gluLoadSamplingMatrices( nurb, model, perspective, view )
%
% C function:  void gluLoadSamplingMatrices(GLUnurbs* nurb, const GLfloat* model, const GLfloat* perspective, const GLint* view)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluLoadSamplingMatrices', nurb, moglsingle(model), moglsingle(perspective), int32(view) );

return
