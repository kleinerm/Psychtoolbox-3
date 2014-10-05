function pipelines = glCreateProgramPipelines( n )

% glCreateProgramPipelines  Interface to OpenGL function glCreateProgramPipelines
%
% usage:  pipelines = glCreateProgramPipelines( n )
%
% C function:  void glCreateProgramPipelines(GLsizei n, GLuint* pipelines)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

pipelines = uint32(0);

moglcore( 'glCreateProgramPipelines', n, pipelines );

return
