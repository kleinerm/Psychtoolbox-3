function [ out ] = GetNTP( com, arg, dbg )
% GetNTP - Query time from NTP server
% 
% socket = GetNTP( 'open', 'hostname' )
% pkg = GetNTP( 'read', socket[, dbg ] )
% GetNTP( 'close', socket )
%
% GetNTP is a very simple pnet based NTP client to query time from an NTP
% server over the network.
%
% A pnet socket has to be created with the open command first. The read
% command returns a structure with the fields timestamps, delay and rtt.
% pkg.timestamps is a vector of four timestamps reflecting client send
% time, server receive time, server transmit time, and client receive time.
% pkg.delay is the network delay as specified by the NTP specification.
% pkg.rtt is the round-trip-time.
%
% If dbg is true the NTP header is decoded completely and added to the
% returned structure including the NTP server's reference timestamp for
% debugging. Additionally server receive and transmit timestamps are
% printed to the console in human readable format.
% 
% Note that any argument checking and error handling was omitted
% intentionally to improve timing with NetStation synchronization.
%
% Reference: Mills, D. L. (2006). Network Time Protocol Version 4
% Reference and Implementation Guide. Retrieved May 21, 2017 from
% https://www.eecis.udel.edu/~mills/database/reports/ntp4/ntp4.pdf
%
% Author: Andreas Widmann, University of Leipzig, 2017

% History:
% 2017-05-25 AW Written.
% 2017-07-19 AW Reformatted help text.

% Copyright (C) 2017 Andreas Widmann, University of Leipzig, widmann@uni-leipzig.de
%
% MIT license:
%
% Permission is hereby granted, free of charge, to any person obtaining a
% copy of this software and associated documentation files (the
% "Software"), to deal in the Software without restriction, including
% without limitation the rights to use, copy, modify, merge, publish,
% distribute, sublicense, and/or sell copies of the Software, and to permit
% persons to whom the Software is furnished to do so, subject to the
% following conditions:
%
% The above copyright notice and this permission notice shall be included
% in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
% OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
% MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
% NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
% DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
% OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
% USE OR OTHER DEALINGS IN THE SOFTWARE.

switch com

    case 'open'
        out = pnet( 'udpsocket', 3333 );
        pnet( out, 'setreadtimeout', 1 );
        pnet( out, 'udpconnect', arg, 123 );

    case 'close'
        pnet( arg, 'close' );

    case 'read'
        
        % Timing sensitive
        msg = uint8( [ 27 zeros(1, 47) ] );
        pnet( arg, 'write', msg);
        out.timestamps( 1 ) = GetSecs;
        pnet( arg, 'writepacket' );
        pkgsize = pnet( arg, 'readpacket' );
        out.timestamps( 4 ) = GetSecs;
        
        % Timing insensitive
        if pkgsize < 48
            error( 'No NTP package received.' );
        end
        
        % Header
        lvm = pnet( arg, 'read', 1, 'uint8');
        % Decode header only for potential KoD (LI == 3) package or on request
        if lvm > 191 || ( nargin > 2 && dbg )
            lvm = dec2bin( lvm, 8 );
            out.LI = bin2dec( lvm( 1:2 ) );
            out.VN = bin2dec( lvm( 3:5 ) );
            out.Mode = bin2dec( lvm( 6:8 ) );
            
            out.stratum = pnet( arg, 'read', 1, 'uint8');
            out.poll = pnet( arg, 'read', 1, 'int8');
            out.prec = pnet( arg, 'read', 1, 'int8');
            % Root delay and dispersion; signed fixed point; decoding to be implemented, possibly
            out.root = pnet( arg, 'read', 2, 'uint32'); 
            
            id = pnet( arg, 'read', 4, 'uint8');
            if out.stratum == 0 || out.stratum == 1
                out.id = char( id );
            else
                out.id = sprintf( '%d.%d.%d.%d', id );
            end
            
            if out.LI == 3 && out.stratum == 0
                error( 'KoD package received. Reason: %s.', out.id );
            end
        else
            pnet( arg, 'read', 15, 'uint8');
        end

        % Timestamps
        timestamps = double( pnet( arg, 'read', 8, 'uint32') );
        timestamps = timestamps( :, 1:2:7 ) + timestamps( :, 2:2:8 ) / 2 ^ 32;
        
        % Receive and transmit timestamps
        out.timestamps( 2:3 ) = timestamps( 3:4 );
        out.delay = ( out.timestamps( 4 ) - out.timestamps( 1 ) ) - ( out.timestamps( 3 ) - out.timestamps( 2 ) );
        out.rtt = out.timestamps( 4 ) - out.timestamps( 1 );

        if nargin > 2 && dbg
            % Reference timestamp
            out.reference = timestamps( 1 ); 
            for iTimestamp = 2:3
                % From modified NetStation.m by Justin and Mario:
                matlabDateNum = datenum( [ 1900 1 1 0 0 out.timestamps( iTimestamp ) ] );
                datestr( matlabDateNum, 'yyyy-mm-dd HH:MM:SS.FFF' )
            end
        end
end
