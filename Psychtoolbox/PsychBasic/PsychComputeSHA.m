function rethash = PsychComputeSHA(msg, shaId)
% hash = PsychComputeSHA(msg [, shaId='256']) -- Return the SHA hash of a string.
%
% This uses either GNU/Octave's builtin hash() function, or if that doesn't exist,
% e.g., on Matlab or old Octave versions, falls back to Java's MessageDigest
% hashing classes to do the same in a slightly less efficient way.
%
% 'msg' should be a string of which the hash should be computed and returned.
%
% 'shaId' optional string with numbers, selecting one of the supported SHA
% functions. This defaults to SHA-256, ie. shaId == '256', if omitted.
%
% Currently the following shaId's are supported by both Octave and Matlab:
% '1', '224', '256', '384', '512'
%
% Returns 'hash' as the string with the returned SHA hash.
%

% History:
% 09-Nov-2020   mk    Written.

if nargin < 2 || isempty(shaId)
    shaId = '256';
end

if exist('hash', 'builtin')
    % Octave builtin
    rethash = hash(['SHA' shaId], msg);

    % Verify against Java implementation?
    if 0
        % Java variant also works with Octave if a JVM is installed, but the Java
        % method has a tad more overhead on any system:
        md = javaMethod('getInstance', 'java.security.MessageDigest', ['SHA-' shaId]);
        refhash = sprintf('%2.2x', typecast(md.digest(uint8(msg)), 'uint8')');
        matching = strcmp(refhash, rethash)
    end
else
    md = javaMethod('getInstance', 'java.security.MessageDigest', ['SHA-' shaId]);
    rethash = sprintf('%2.2x', typecast(md.digest(uint8(msg)), 'uint8')');
end

return;
