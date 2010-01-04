function rc = PsychGPUControl(cmd, varargin)
% rc = PsychGPUControl(cmd, arg); -- Control low-level GPU settings.
%
% PsychGPUControl calls into external helper tools to change certain
% low-level operating settings of your systems graphics card (GPU).
%
% Not all operating systems and GPU's support this. The function will
% do nothing on unsupported OS/GPU combos.
%
% Currently OS/X doesn't support this function at all, and on MS-Windows
% and GNU/Linux, only recent ATI GPU's with recent drivers do support it.
%
% All subfunctions return an optional 'rc' return code of zero on success,
% non-zero on error or if the feature is unsupported.
%
% Subfunctions and their syntax & meaning:
%
% rc = PsychGPUControl('SetDitheringEnabled', enableFlag);
% - Depending on the setting of 'enableFlag', either enable (=1) or
% disable (=0) display color dithering on all connected displays.
%
% Under normal circumstances, the GPU should decide itself if dithering
% should be used or not. This function allows you to override the GPU's
% automatic choice.
%
%
% rc = PsychGPUControl('SetGPUPerformance', gpuPerformance);
% - Select the performance state of the GPU. 'gpuPerformance' can be set to
% 0 if the GPU shall automatically adjust its performance and power-
% consumption, or to one of 10 fixed levels between 1 and 10, where 1 means
% the lowest performance - and power consumption, whereas 10 means the
% highest performance - and maximum power consumption.
%
% If in doubt, choose 10 for best accuracy of visual stimulus onset timing,
% 0 for non-critical activities to leave the decision up to the graphics
% driver and GPU.
%
%

% History:
%  3.01.2010  mk  Written.

if nargin < 1
	error('Subfunction command argument missing!');
end

if strcmpi(cmd, 'SetDitheringEnabled')
	if isempty(varargin)
		error('SetDitheringEnabled: enableFlag argument missing!');
	end

	enable = varargin{1};
	if ~ismember(enable, 0:1)
		error('SetDitheringEnabled: Invalid enableFlag argument, not 0 or 1!');
	end

	% Command code 1 means: Control ditherstate, according to 2nd arg 0 or 1 == disable, enable.
	cmdpostfix = sprintf(' 1 %i', enable);
	rc = executeRadeoncmd(cmdpostfix);
	return;
end

if strcmpi(cmd, 'SetGPUPerformance')
	if isempty(varargin)
		error('SetGPUPerformance: gpuPerformance argument missing!');
	end

	gpuperf = varargin{1};
	if ~ismember(gpuperf, 0:10)
		error('SetGPUPerformance: Invalid gpuPerformance argument, not an integer in range 0 - 10!');
	end

	% Map range 1 to 5 to "minimum performance" on ATI GPU's:
	if gpuperf > 0 & gpuperf <= 5
		perfflag = 2;
	end

	% Map range 6 to 10 to "maximum performance" on ATI GPU's:
	if gpuperf > 5 & gpuperf <= 10
		perfflag = 1;
	end

	if gpuperf == 0
		perfflag = 0;
	end

	% Command code 0 means: Control performance state: 0 = AUTO, 1 = MAX, 2 = MIN.
	cmdpostfix = sprintf(' 0 %i', perfflag);
	rc = executeRadeoncmd(cmdpostfix);
	return;
end

error('Invalid subfunction provided. Read the help for valid commands!');
return;

function rc = executeRadeoncmd(cmdpostfix)
	% Default to a return code of 1 for success:
	if IsOSX
		% A no-op on OS/X, as this is not supported at all.
		rc = 1;
		return;
	end

	if IsLinux
		cmdprefix = '/PsychContributed/ATIRadeonperf_Linux ';
	end

	if IsWin
		cmdprefix = '/PsychContributed/ATIRadeonperf_Windows ';
	end

	% Call final command, return its return status code:
	rc = system([PsychtoolboxRoot cmdprefix cmdpostfix]);

	return;
end
