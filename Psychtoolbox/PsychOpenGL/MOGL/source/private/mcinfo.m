function [ M, C ] = mcinfo( funcp )

% MCINFO  From a parsed C declaration, extract information needed to
%         write M-file wrapper and C interface function
% 
% [ M, C ] = mcinfo( funcp )

% 24-Jan-2005 -- created;  adapted from code in autocode.m (RFM)

% make lowercase function name, e.g., glVertex3dv --> gl_vertex3dv
k=min(find(funcp.fname~=lower(funcp.fname)));
lowerfn=lower([ funcp.fname(1:k-1) '_' funcp.fname(k:end) ]);

% initialize properties of C function
C.interfacename=lowerfn;    % name of interface function
C.arg_in={};                % input arguments
C.arg_out={};               % return argument

% initialize proerties of M-file wrapper
M.fname=funcp.fname;        % M-file filename
M.arg_in={};                % input arguments
M.arg_in_check={};          % code to check input arguments
M.arg_out={};               % return arguments
M.arg_out_init={};          % code to initialize return arguments
M.mogl_in={};               % input arguments in call to moglcore
M.mogl_out={};              % return arguments in call to moglcore
M.allocate=0;		        % flag indicating whether arguments to
							% moglcore.mexmac must be allocated manually

% if return type of function declaration is not void, then add return
% arguments to C function and M-file
if strcmp(funcp.argouttype.basetype,'void')==0,
	% scalar return argument
	if isempty(funcp.argouttype.stars),
		C.arg_out{1}=sprintf('plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);\n\t*mxGetPr(plhs[0])=(double)');
		M.arg_out{1}='r';
		M.mogl_out{1}='r';
	% pointer return argument
	else
		C.arg_out{1}=sprintf('plhs[0]=mxCreateNumericMatrix(1,1,mxUINT32_CLASS,mxREAL);\n\t*(unsigned int *)mxGetData(plhs[0])=(unsigned int)');
		M.arg_out{1}='r';
		M.mogl_out{1}='r';
	end
else
	C.arg_out{1}='';
end

% step through input arguments
for j=1:numel(funcp.argin.args),

	% skip void input arguments
	if strcmp(funcp.argin.args(j).type.full,'void'),
		continue
	end

	% provide an argument name, if none specified in declaration
	if isempty(funcp.argin.args(j).argname),
		funcp.argin.args(j).argname=sprintf('arg%d',j);
	end

	% scalar input argument?
	if isempty(funcp.argin.args(j).type.stars),
		C.arg_in{end+1}=sprintf('(%s)mxGetScalar(prhs[%d])',funcp.argin.args(j).type.full,j-1);
		M.arg_in{end+1}=funcp.argin.args(j).argname;
		M.mogl_in{end+1}=funcp.argin.args(j).argname;

	% honourary scalar?  (i.e., pointer to a GLU struct)
	elseif ~isempty(strfind(funcp.argin.args(j).type.basetype,'GLU')),
		C.arg_in{end+1}=sprintf('(%s)(unsigned int)mxGetScalar(prhs[%d])',funcp.argin.args(j).type.full,j-1);
		M.arg_in{end+1}=funcp.argin.args(j).argname;
		M.arg_in_check{end+1}=sprintf('if ~strcmp(class(%s),''uint32''),\n\terror([ ''argument ''''%s'''' must be a pointer coded as type uint32 '' ]);\nend\n',funcp.argin.args(j).argname,funcp.argin.args(j).argname);
		M.mogl_in{end+1}=funcp.argin.args(j).argname;

	% pointer?
	else

		% decide how to cast input argument, if necessary
		switch funcp.argin.args(j).type.basetype,
			case { 'GLdouble' 'GLclampd' }
				mcast='double';
			case { 'GLfloat' 'GLclampf' }
				mcast='moglsingle';
			case { 'GLint' 'GLsizei' }
				mcast='int32';
			case { 'GLuint' 'GLenum' 'GLbitfield' 'GLUnurbs' 'GLUtesselator' 'GLUquadric'}
				mcast='uint32';
			case { 'GLshort' }
				mcast='int16';
			case { 'GLushort' }
				mcast='uint16';
			case { 'GLbyte' }
				mcast='int8';
			case { 'GLubyte' 'GLchar' 'GLboolean' }
				mcast='uint8';
			case { 'GLvoid' 'void' }
				mcast='';
            case { 'GLhandleARB' 'GLhandle'}
                mcast='void*';
			otherwise
				error(sprintf('don''t know how to handle pointer type %s',funcp.argin.args(j).type.full));
		end

		% add input argument to C function
		C.arg_in{end+1}=sprintf('(%s)mxGetData(prhs[%d])',funcp.argin.args(j).type.full,j-1);

		% check wither pointer is const;  if not, assume data will be
		% written to this input argument, so initialize it
		if isempty(funcp.argin.args(j).type.const),
			thisallocate=1;
			M.allocate=1;
			M.arg_out{end+1}=funcp.argin.args(j).argname;
			M.arg_out_init{end+1}=sprintf('%s = %s(0);',funcp.argin.args(j).argname,mcast);
		else
			thisallocate=0;
			M.arg_in{end+1}=funcp.argin.args(j).argname;
		end

		% if pointer is not void, and is const, then typecast argument
		if ~isempty(mcast) & ~thisallocate,
			M.mogl_in{end+1}=sprintf('%s(%s)',mcast,funcp.argin.args(j).argname);
		else
			M.mogl_in{end+1}=sprintf('%s',funcp.argin.args(j).argname);
		end

	end

end

return
