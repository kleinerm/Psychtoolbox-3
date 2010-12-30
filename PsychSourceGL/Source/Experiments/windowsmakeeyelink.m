% go here and install visual studio c/c++ express edition (free)
% http://www.microsoft.com/express/download/#webInstall
%
% from wikipedia on visual studio express edition:
% "Visual C++ 2008 Express can build both native and managed applications. Included is the Windows Platform SDK which can build applications that use the Win32 API.
% Applications utilizing either MFC or ATL require the Standard Edition or higher, and will not compile with the Express Edition."
%
% version 2008 (aka v9.0) with sp1 seems to work (it is listed as supported for mex here http://www.mathworks.com/support/tech-notes/1600/1601.html), at least in matlab 2008a
%
% in matlab, choose this compiler via:
% mex -setup
%
% make sure the paths below are set correctly

function windowsmakeeyelink(postR2007a,env)
% windowsmakeeyelink([postR2007a=0],[env])
% windowsmakeeyelink -- Simple build-script for building
% Eyelink toolbox for M$-Windows:
%
% postR2007a == 0 --> Build on Matlab R11. --> default
% postR2007a == 1 --> Build on Matlab R2007a.
%
% env ==
%  'tuebingen' --> default
%  'eflister'

if nargin < 1
	postR2007a = [];
end

if isempty(postR2007a)
	postR2007a = 0;
end

postR2007a %#ok<NOPRT>

if ~exist('env','var') 
	env=[];
end

if isempty(env)
	env='tuebingen';
end

eyelinkPath='C:\Program Files\SR Research\EyeLink\'; %parent of libs and Includes directory
switch env %update these for your situation
	case 'tuebingen'
		if postR2007a
			%Build for >= R2007a Matlabs on MPI HDR machine (2007a)
			path='T:\projects\OpenGLPsychtoolbox\trunk\'; %parent of PsychSourceGL directory
			compilerPath='C:\Programme\Microsoft Visual Studio 8\VC\Include'; %compiler-provided headers
		else
			%Build for pre R2007a Matlabs on MKs Laptop (R11)
			path='T:\kleinerm\trunk\'; %parent of PsychSourceGL directory
			eyelinkPath='C:\Programme\SRResearch\EyeLink'; %parent of eyelink libs and Includes directory
			quicktimePath='C:\Programme\QuickTimeSDK\CIncludes'; %directory of quicktime headers
			compilerPath='C:\Programme\MicrosoftVisualStudio\VC98\Include'; %compiler-provided headers
		end
	case 'eflister'
		if ~postR2007a
			error('eflister only uses post 2007a')
		end
		path='C:\Documents and Settings\rlab\Desktop\ptb dev branch\trunk\'; %parent of PsychSourceGL directory
		compilerPath='C:\Program Files\Microsoft Visual Studio 9.0\VC\include'; %compiler-provided headers
	otherwise
		error('unrecognized env')
end

% *****************************
%nothing else below should need customization

requireds={'path', 'eyelinkPath', 'compilerPath'};
%this line is cool but mario's old matlab doesn't have cellfun
%if any(1~=cellfun(@exist,requireds)) || any(cellfun(@isempty,requireds)) || any(7~=cellfun(@exist,cellfun(@eval,requireds,'UniformOutput',false)))
for i=1:length(requireds)
	problem=~exist(requireds{i},'var');
	if ~problem
%		problem=isempty(requireds{i}) | ~exist(eval(requireds{i}),'dir'); %#ok<OR2> %mario's old matlab doesn't have shortcircuiting ops
	end
	if problem
		error('must set %s to a valid directory',requireds{i})
	end
end

buildPath=fullfile(path,'PsychSourceGL\Projects\Windows\build');
if ~exist(buildPath,'dir')
	error('bad path -- should be parent of PsychSourceGL directory')
end

commonBasePath=fullfile(path,'PsychSourceGL\Source\Common\Base');
cd(fullfile(commonBasePath,'..\..\'))

% Copy our C++ PsychScriptingGlue.cc to a C style PsychScriptingGlue.c so the f%*§$!d M$-Compiler can handle it:
copyfile(fullfile(commonBasePath,'PsychScriptingGlue.cc'),fullfile(commonBasePath,'PsychScriptingGlue.c'))

%buildPath=fullfile(path,'PsychSourceGL\Projects\Windows\build');
eyelinkIncludesPath=fullfile(eyelinkPath,'Includes\eyelink');
eyelinkLibsPath=fullfile(eyelinkPath,'libs');

%if any(7~=cellfun(@exist,{eyelinkIncludesPath,eyelinkLibsPath}))
for i={eyelinkIncludesPath,eyelinkLibsPath}
	if ~exist(i{1},'dir')
		error('bad eyelinkPath -- should be parent of eyelink ''libs'' and ''Includes'' directories')
	end
end

% Build sequence for Eyelink.dll: Requires the freely downloadable (after registration) Eyelink-SDK for Windows.
mexCmd=sprintf('mex -v -outdir ''%s'' -output Eyelink -I"%s" -I"%s" -ICommon\\Base -ICommon\\Eyelink -IWindows\\Base Windows\\Base\\*.c Common\\Base\\*.c Common\\Eyelink\\*.c user32.lib gdi32.lib advapi32.lib winmm.lib "%s" "%s" "%s"',buildPath,eyelinkIncludesPath,compilerPath,fullfile(eyelinkLibsPath,'eyelink_core.lib'),fullfile(eyelinkLibsPath,'eyelink_w32_comp.lib'),fullfile(eyelinkLibsPath,'eyelink_exptkit20.lib'));
if ~postR2007a
	problem = ~exist('quicktimePath','var');
	if ~problem
		problem= isempty(quicktimePath) | ~exist(quicktimePath,'dir'); %#ok<OR2>
	end
	if problem
		error('pre 2007a must set quicktimePath to a valid directory (edf wonders why it needs quicktime?)')
	end
	
	mexCmd=sprintf('%s -I"%s"',mexCmd,quicktimePath);
end
eval(mexCmd)

% Move Eyelink.dll into its proper location:
movefile(fullfile(buildPath,'Eyelink.mexw32'),fullfile(path,'Psychtoolbox\PsychBasic\MatlabWindowsFilesR2007a\'))

% Delete the temporary .c version of Scripting Glue:
delete(fullfile(commonBasePath,'PsychScriptingGlue.c'))
end
