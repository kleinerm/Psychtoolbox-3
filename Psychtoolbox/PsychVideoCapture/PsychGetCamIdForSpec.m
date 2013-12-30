function [devid, dev] = PsychGetCamIdForSpec(className, inputNameOrPort, instance, engineId)
% Return deviceIndex of a specified camera, one that matches given criteria.
% 
% [deviceIndex, dev] = PsychGetCamIdForSpec([className][, inputNameOrPort][, instance][, engineId]);
%
% Searches for video sources which match given criteria. A handle to the
% first source that satisfies the criteria is returned in argument
% 'deviceIndex' - You can open a connection to the source via
% Screen('OpenVideoCapture', windowPtr, deviceIndex, ...);
%
% The 2nd optional return argument contains the complete 'dev'ice
% description struct, as returned by Screen().
%
% Returns empty variables if no match can be made.
%
% Optional criteria:
%
% 'className' Index or name of video input device class: Default is to
% accept any class. This matches against the 'ClassName' property of the
% list returned by Screen('VideoCaptureDevices').
%
% 'inputNameOrPort' selects the i'th input device of a matching class if an
% index is given (zero-based), or a specific named device, e.g., 'iSight'
% for the builtin iSight camera of Apple hardware.
%
% 'instance' If multiple devices match, take the i'th device where i ==
% instance. By default, the first device (instance == 0) is assigned.
%
% 'engineId' Enumerate for video capture engine 'engineId'. By default, the
% default videocapture engine is used.
%

%
% History:
% 9.5.2009  mk Written.

if nargin < 1
    className = [];
end

if nargin < 2
    inputNameOrPort = [];
end

if nargin < 3
    instance = [];
end

if isempty(instance)
    instance = 0;
end

if nargin < 4
    engineId = [];
end

cams = Screen('VideoCaptureDevices', engineId);
devid = [];
dev = [];

curinst = -1;

for i=1:length(cams)
    if ~isempty(className)
        if isnumeric(className)
            if cams(i).ClassIndex ~= className
                % ClassIndex doesn't match: Reject.
                continue;
            end
        else
            if isempty(strfind(cams(i).ClassName, className))
                % Classname doesn't match: Reject.
                continue;
            end
        end
    end
    
    if ~isempty(inputNameOrPort)
        if isnumeric(inputNameOrPort)
            if cams(i).InputIndex ~= inputNameOrPort
                % InputIndex doesn't match: Reject.
                continue;
            end
        else
            if isfield(cams(i), 'InputName') && isempty(strfind(cams(i).InputName, inputNameOrPort))
                % InputName doesn't match: Reject.
                continue;
            end

            if isfield(cams(i), 'DeviceName') && isempty(strfind(cams(i).DeviceName, inputNameOrPort))
                % InputName doesn't match: Reject.
                continue;
            end
        end
    end
    
    % Matching criteria satisfied. Matching instance?
    curinst = curinst + 1;
    
    if curinst == instance
        % Assign deviceIndex for matched device:
        devid = cams(i).DeviceIndex;
        dev = cams(i);
        break;
    end
end

% Ok, either devid is empty if no matching device could be found, or we
% have a unique deviceIndex for use with Screen('OpenVideoCapture').

return;
