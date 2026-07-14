function  time = EyelinkClearMsgQueue( period , thistime )
%
% EyelinkClearMsgQueue
% EyelinkClearMsgQueue( period )
% EyelinkClearMsgQueue( period , thistime )
%
% time = EyelinkClearMsgQueue( ... )
%
% Windowing environments try to detect the responsiveness of an application
% based upon whether its windows' events are being actively consumed with
% some regularity. A window is deemed to be unresponsive by MS Windows when
% its message queue has not been read from. This can occur during tracker
% setup, or whilst waiting for the subject to respond during a trial in
% which Screen does not regularly Flip.  This problem is especially
% pronounced in MS Windows, because the supposedly unresponsive application
% may be terminated without warning.
%
% EyelinkClearMsgQueue can clear the message queue by polling the state of
% the mouse using GetMouse. This will happen at most once every 'period'
% seconds (default 1). The function will call GetSecs to measure the time
% since the previous call to GetMouse, unless 'thistime' is provided; the
% value of 'thistime' will take precedence over GetSecs.
%
% Returns 0 (zero) when less than 'period' seconds have passed since the
% last call to GetMouse. Otherwise, the time measured by GetSecs or
% 'thistime' is returned.
%
% The function will only take effect when called in MS Windows. No action
% is taken in any other platform, other than to return 0 (zero).
%
    
    % isWindows  Remembers whether the OS is MS Windows or not.
    % cleartime  The last time when GetMouse was called to clear the
    %            message queue.
    persistent  isWindows
    persistent  cleartime

    % This is the first call to EyelinkClearMsgQueue.
    %
    if  isempty( isWindows )
        isWindows = IsWin ;
        cleartime = 0 ;
    end

    % Set default return value.
    %
    time = 0 ;

    % The platform is not presently MS Windows.
    %
    if  ~isWindows ,  return ,  end

    % Set default period, if not provided.
    %
    if  nargin < 1 ,  period = 1 ;  end

    % Measure time, if not provided.
    %
    if  nargin < 2 ,  thistime = GetSecs ;  end

    % The period has passed since the last time that the message queue was
    % cleared.
    %
    if  thistime - cleartime >= period
        GetMouse ;
        cleartime = thistime ;
        time = thistime ;
    end
    
end  % EyelinkClearMsgQueue
