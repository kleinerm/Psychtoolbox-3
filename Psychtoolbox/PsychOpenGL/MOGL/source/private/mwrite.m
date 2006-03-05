function mwrite( funcp, M )

% MWRITE  Write an M-file wrapper function
%
% mwrite( funcp, M )

% 24-Jan-2005 -- created;  adapted from code in autocode.m (RFM)

% set names for temporary files
tmpwrapfile1='/tmp/mogl_wrap1.m';
tmpwrapfile2='/tmp/mogl_wrap2.m';

% write M-file wrapper to a temporary file
fid=fopen(tmpwrapfile1,'w');
fndef='';
% M-file output arguments
if numel(M.arg_out)==1,
	fndef=[ fndef sprintf('%s = ',M.arg_out{1}) ];
elseif numel(M.arg_out)>1,
	fndef=[ fndef sprintf('[ %s ] = ',commalist(M.arg_out{:})) ];
end
% M-file function name
fndef=[ fndef funcp.fname ];
% M-file input arguments
if ~isempty(M.arg_in),
	fndef=[ fndef sprintf('( %s )',commalist(M.arg_in{:})) ];
end
% write
fprintf(fid,'function %s\n\n',fndef);
fprintf(fid,'%% %s  Interface to OpenGL function %s\n%%\n',funcp.fname,funcp.fname);
fprintf(fid,'%% usage:  %s\n%%\n',fndef);
fprintf(fid,'%% C function:  %s\n\n',funcp.full);
fprintf(fid,'%% %s -- created (generated automatically from header files)\n\n',datestr(now,1));
% flag that at least one input argument to moglcore call is modified,
% and allocation code must be checked manually
if M.allocate,
	fprintf(fid,'%% ---allocate---\n\n');
end
% check number of arguments
fprintf(fid,'if nargin~=%d,\n',numel(M.arg_in));
fprintf(fid,'    error(''invalid number of arguments'');\n');
fprintf(fid,'end\n\n');
% check type of arguments
if ~isempty(M.arg_in_check),
	fprintf(fid,'%s',M.arg_in_check{:});
	fprintf(fid,'\n');
end
% init return arguments
if M.allocate,
	for k=1:numel(M.arg_out_init),
		fprintf(fid,'%s\n',M.arg_out_init{k});
	end
	fprintf(fid,'\n');
end
% call to moglcore
if ~isempty(M.mogl_out),
	fprintf(fid,'%s = ',M.mogl_out{1});
end
fprintf(fid,'moglcore( %s );\n\n',commalist(sprintf('''%s''',funcp.fname),M.mogl_in{:}) );
fprintf(fid,'return\n');
fclose(fid);

% see whether existing M-file wrapper is protected
wrapfile=sprintf('../wrap/%s.m',funcp.fname);
mprotected=filecontains(wrapfile,'---protected---');

% if protected, append automatically generated code as comments
if mprotected,

	% strip existing autocode
	if filecontains(wrapfile,'---autocode---'),
		unix(sprintf('sed -E -n ''1,/---autocode---/p'' %s > %s',wrapfile,tmpwrapfile2));
		unix(sprintf('mv %s %s',tmpwrapfile2,wrapfile));
		% if no autocode, write autocode header
	else
		unix(sprintf('echo >> %s',wrapfile));
		unix(sprintf('echo >> %s',wrapfile));
		unix(sprintf('echo ''%% ---autocode---'' >> %s',wrapfile));
	end

	% append automatically generated code as comments
	unix(sprintf('echo ''%%'' >> %s',wrapfile));
	unix(sprintf('cat %s | sed -E ''s/^/%% /'' >> %s',tmpwrapfile1,wrapfile));
	unix(sprintf('echo ''%%'' >> %s',wrapfile));

	% otherwise, just overwrite the existing file
else

	unix(sprintf('rm %s 2> /dev/null',wrapfile));
	if M.allocate,
		wrapfile=sprintf('../wrap/_%s.m',funcp.fname);
	end
	unix(sprintf('mv %s %s',tmpwrapfile1,wrapfile));

end

return
