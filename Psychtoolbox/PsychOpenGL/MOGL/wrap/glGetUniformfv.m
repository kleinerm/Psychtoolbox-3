function params = glGetUniformfv( program, location )

% glGetUniformfv  Interface to OpenGL function glGetUniformfv
%
% usage:  params = glGetUniformfv( program, location )
%
% C function:  void glGetUniformfv(GLuint program, GLint location, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=2,
    error('invalid number of arguments');
end

% Allocate enough space of float single datatype for a sufficient
% number of arguments (worst case): I believe the biggest return could be a
% 4 x 4 matrix, i.e. 16 elements, but i'm not sure about arrays...
% FIXME: Verify this assumption!
params = single(repmat(NaN,[ 16 1 ]));

moglcore( 'glGetUniformfv', program, location, params );
params = double(params);
params = params(find(~isnan(params))); %#ok<FNDSB>

return
