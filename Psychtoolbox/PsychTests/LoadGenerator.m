function LoadGenerator(yieldmsecs)
% LoadGenerator([yieldmsecs=0])
%
% Create cpu-load by simply executing an empty while-loop as fast as
% possible.
%
% Optionally the load can be reduced by specifying a non-zero 'yieldmsecs'
% argument telling the function that it should release the cpu for
% 'yieldmsecs' milliseconds in each loop iteration.
%
% This function never returns, you have to abort it by pressing CTRL+C.
%

    % Default to a overload load:
    if nargin < 1
        yieldmsecs = 0;
    end
    
    % Repeat forever:
    while 1
        % If yield requested, yield for given number of msecs, otherwise
        % just spin-wait at 100% of achievable cpu load:
        if yieldmsecs > 0
            WaitSecs('YieldSecs', yieldmsecs / 1000);
        end
    end
    % Done.
end
