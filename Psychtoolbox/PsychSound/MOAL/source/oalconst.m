function oalconst(glheaderpath, aglheaderpath)

% OGLCONST  Collect AL, ALU, and AAL constants from C header files, and
%           store them in oalconst.mat
%
% usage:  oalconst

% 09-Dec-2005 -- created (RFM)
% 23-Jan-2005 -- constants saved in both struct and OpenGL style (RFM)
% 05-Mar-2006 -- ability to spec. system header file path added (MK)
% 05-Feb-2007 -- OpenAL version, derived from OpenGL version (MK)

if IsWin
    error('Parsing of AL header files on Windows not yet supported.');
end;

% Alternate path to header files specified?
if nargin < 1
    if IsOSX
        glheaderpath = '/System/Library/Frameworks/OpenAL.framework/Headers';
    end;

    if IsLinux
        glheaderpath = '/usr/include/AL';
    end;
end;

if nargin < 2
    if IsOSX
        aglheaderpath = '/System/Library/Frameworks/AGL.framework/Headers';
    end;
end;

if IsOSX
    fprintf('Parsing OpenAL, ALC and AAL header files in %s and %s ...\n',glheaderpath, aglheaderpath);
else
    fprintf('Parsing OpenAL and ALC header files in %s ...\n',glheaderpath);
end;

% get constants from the OpenGL header files.  the 'parsefile' routine
% defines the constants in the calling workspace, as variables with the
% same names as the #defined OpenAL constants, e.g., AL_COLOR_BUFFER_BIT.
% the return argument contains all the constants as fields of a structure,
% e.g., AL.COLOR_BUFFER_BIT.  (note that the first underscore has been
% changed to a period.)  the first style is more like the C convention,
% and the second style prevents the workspace from being swamped with
% hundreds of global variables.  later on, we can load whichever style
% we want from the file where they're all saved.
AL= parsefile(sprintf('%s/al.h', glheaderpath), 'AL_');
%AL= parsefile(sprintf('%s/alext.h', glheaderpath), 'AL_', AL);
ALC=parsefile(sprintf('%s/alc.h', glheaderpath),'ALC_');
if IsOSX
%    AAL=parsefile(sprintf('%s/aal.h', aglheaderpath),'AAL_');
end;

fname='oalconst.mat';

% save OpenGL-style constants
if IsOSX
    save(fname,'AL_*','ALC_*', '-V6');
    % save structure-style constants to same file
    save(fname,'AL','ALC','-append', '-V6');
end;

if IsLinux
    save(fname,'AL_*','ALC_*','-V6');
    % save structure-style constants to same file
    save(fname,'AL','ALC','-append','-V6');
end;

% put a copy into the 'core' directory
copyfile(fname,'../core');

return


% function to parse header files
function S = parsefile( fname, prefix, origin )

% initialize return argument
if nargin < 3
    S=[];
else
    S=origin;
end;

% check size of prefix (AL, ALC, or AAL)
nprefix=length(prefix);

% open input file
fid=fopen(fname,'r');

% step through lines of input file
while ~feof(fid),

    % read a line and parse it as '#define SYMBOL VALUE'
    codeline=fgets(fid);
    r=regexp(codeline,'^\s*#define\s+(?<symbol>\S+)\s*(?<value>\S*)','names');

    % if it's not a #define statement, then skip it
    if isempty(r),
        continue
    end

    % if the symbol doesn't have the required prefix,
	% then skip it
    if ~strncmp(r.symbol,prefix,nprefix),
        continue
    end

    % remove prefix from symbol
	fieldname=r.symbol((nprefix+1):end);
    
    % if remainder of symbol name begins with a digit, then add 'N' to make
    % it a valid field name
    if ismember(fieldname(1),'0123456789'),
        fieldname=[ 'N' fieldname ];
    end

    % convert value to a numeric value
    if ~isempty(r.value),
        if strncmp(r.value,'0x',2),
			% convert hex value
            nvalue=hex2dec(r.value(3:end));
		else
			% convert decimal value
            nvalue=str2num(r.value);
        end
        % assign value of zero if conversion failed, e.g., if symbol was
		% defined using another #define, as in #define GL_THIS GL_THAT.
		% if anything important is #defined this way, we'll have to fix
		% up this part.
        if isempty(nvalue),
            warning('error converting numeric value from line:  %s',codeline);
            nvalue=0;
        end
	else
		% assign zero if symbol has no value, e.g., #define GL_I_WAS_HERE
        nvalue=0;
    end

    % add numeric value to struct
    fprintf(1,'%s\t%-20s\t%s\t%.0f\n',prefix,fieldname,r.value,nvalue);
    S=setfield(S,fieldname,nvalue);
    
    % define OpenAL-style variable in calling workspace
	evalin('caller',sprintf('%s=%d;',r.symbol,nvalue));

end

% close file
fclose(fid);

return
