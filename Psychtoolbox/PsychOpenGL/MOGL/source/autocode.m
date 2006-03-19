function autocode(glheaderpath)

% AUTOCODE.M  Generate MATLAB-OpenGL interface code from OpenGL header files
%
% Usage: autocode(glheaderpath).
% glheaderpath is the path to the OpenGL gl.h and glu.h header files.
% It defaults to /System/Library/Frameworks/OpenGL.framework/Headers/
%
% 18-Dec-05 -- created (RFM)
% 05-Mar-06 -- added option to spec. glheaderpath (MK)

clc;

% Alternate path to header files specified?
if nargin < 1
    if IsOSX
        glheaderpath = '/System/Library/Frameworks/OpenGL.framework/Headers';
    end;
    if IsLinux
        glheaderpath = '/usr/include/GL';
    end;
end;

fprintf('Parsing OpenGL and GLU header files in %s ...\n',glheaderpath);

% delete unprotected wrapper files
% ( protected = contains string "---protected---" somewhere in the file )
! find ../wrap/*.m -not -exec grep -q -e ---protected--- {} \; -print0 | xargs -0 -n 1 rm

% make file with list of OpenGL functions
tmplistfile='/tmp/mogl_listfile.txt';
unix(sprintf('grep gl[A-Z]   %s/gl.h        | grep -v ProcPtr | grep -v \\#define | sed -E ''s/^[[:space:]]*extern[[:space:]]*//'' | sed -E ''s/;[[:space:]]*$//'' >  %s',glheaderpath, tmplistfile));
% MK Disabled for now: unix(sprintf('grep gl[A-Z]   %s/glext.h     | grep -v ProcPtr | grep -v \\#define | sed -E ''s/^[[:space:]]*extern[[:space:]]*//'' | sed -E ''s/;[[:space:]]*$//'' >  %s',glheaderpath, tmplistfile));
unix(sprintf('grep glu[A-Z]  %s/glu.h       | grep -v ProcPtr | grep -v \\#define | sed -E ''s/^[[:space:]]*extern[[:space:]]*//'' | sed -E ''s/;[[:space:]]*$//'' >> %s',glheaderpath, tmplistfile));
unix(sprintf('grep glut[A-Z] headers/glut_edit.h | grep -v ProcPtr | grep -v \\#define | sed -E ''s/^[[:space:]]*extern[[:space:]]*//'' | sed -E ''s/;[[:space:]]*$//'' >> %s',tmplistfile));
listfid=fopen(tmplistfile,'r');

% initialize and open C file
cfile='gl_auto.c';
if unix(sprintf('cat gl_auto_init.c | sed ''s/DATE/%s/'' > %s',datestr(now,1),cfile))~=0,
    error(sprintf('unable to initialize ''%s''',cfile));
end
cfid=fopen(cfile,'a');

% initialize list of entries in command map
cmdmap={};

% step through list of OpenGL functions
while(~feof(listfid)),

    % parse the next function declaration
    funcp=cparse(fgetl(listfid));

    % skip functions in autono.txt
    if filecontains('autono.txt',funcp.fname),
        continue
	end
	
	% skip functions that use double indirection in an input argument
	if doubleindirect(funcp),
		% add function name to autono.txt
		unix(sprintf('cat "%s  %% uses double indirection (added to autono.txt by autocode.m) >> autono.txt',funcp.fname));
		continue
	end
    
	% function meets criteria, so show function name
    fprintf(1,'%s\n',funcp.fname);
	
	% extract information needed to construct M-file wrapper and
	% C interface function (input arguments, return arguments, etc.)
	[M,C]=mcinfo(funcp);

	% create M-file wrapper
	mwrite(funcp,M);

    % create C interface function
	cwrite(cfid,funcp,C);
	
	% add line to command map (to be written to C file later)
	cmdmap{end+1}=sprintf('{ %-35s%-35s }',[ '"' funcp.fname '",' ],C.interfacename);
	
end

% sort and write command map
cmdmap=sort(cmdmap);
fprintf(cfid,'int gl_auto_map_count=%d;\n',numel(cmdmap));
fprintf(cfid,'cmdhandler gl_auto_map[] = {\n');
fprintf(cfid,'%s,\n',cmdmap{1:end-1});
fprintf(cfid,'%s};\n',cmdmap{end});

% close files
fclose(cfid);
fclose(listfid);

% clean up
delete('/tmp/mogl_*');

return
