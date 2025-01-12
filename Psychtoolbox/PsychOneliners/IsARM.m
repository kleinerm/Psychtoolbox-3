function resultFlag = IsARM(wantTrueHostArch)
% resultFlag = IsARM([wantTrueHostArch=0])
%
% By default (if wantTrueHostArch omitted or false) returns true if the
% cpu architecture for which Octave or Matlab and Psychtoolbox mex files
% are built is ARM. Otherwise it returns false.
%
% If wantTrueHostArch is true, then return the true machine processor
% architecture of the host computer and operating system. This matters if
% one is running Psychtoolbox on a non-native Octave or Matlab on a ARM
% operating system + machine via some emulation layer, e.g., most commonly
% Octave/Matlab for Apple Intel Macs on an Apple Silicon Mac via Rosetta2
% emulation.
%
% Note: True machine detection is currently only implemented for macOS,
% otherwise we assume scripting runtime architecture == true architecture
% for now.

% HISTORY
% 04.04.2013 mk   Wrote it.
% 16.12.2023 mk   Simplify and make more robust and recognize Apple Silicon.
% 11.01.2025 mk   Optionally return true architecture of host machine + OS.

persistent rc;
persistent mrc;

if isempty(rc)
    rc = ~isempty(strfind(computer, 'arm-')) || ~isempty(strfind(computer, 'aarch')) || streq(computer, 'MACA64'); %#ok<*STREMP>

    if IsOSX
        % macOS: Rosetta2 emulation allows running Matlab/Octave for 64-Bit
        % Intel to run on 64-Bit ARM, so can't rely on computer() to find
        % out the true machine architecture of the underlying OS and HW:
        [s, resp] = system('uname -a');
        if s == 0
            mrc = ~isempty(strfind(resp, 'ARM64'));
        else
            mrc = rc;
            warning('IsARM(): Could not determine true host system machine architecture [%s]. Going with arch of scripting runtime. This may go sideways!', resp);
        end
    else
        % Others: For now, just assume actual host machine architecture is
        % architecture for which the scripting environment was built:
        mrc = rc;
    end
end

if (nargin == 1) && ~isempty(wantTrueHostArch) && wantTrueHostArch
    resultFlag = mrc;
else
    resultFlag = rc;
end
