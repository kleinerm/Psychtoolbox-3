unit getmac;

(*

  this unit provides the necessary definitions for netbios calls to

  a) reset an ethernet adapter
  b) get adapter status (and thus the mac-id)

  this unit is partially derived from Borland's nb30.pas, other influences
  come from Ralf Brown's Interrupt List and from the
  CBIS Net Bios Programmer's Reference 
  
*)

interface

const
  { size of a netbios name record }
  NCBNameSize = 16;
  { lana's in range 0 to MAX_LANA inclusive }
  MAX_LANA = 254;


type
  { netbios name record }
  TNBNameRecord = packed array[0..NCBNameSize - 1] of Char;

  { Network Control Block }
  TNCB = packed record
    ncb_command: Byte; (* command *)
    ncb_retcode: Byte; (* return code *)
    ncb_lsn: Byte; (* local session number *)
    ncb_num: Byte; (* number assigned to our name *)
    ncb_buffer: Pointer; (* pointer to message buffer *)
    ncb_length: Word; (* buffer size *)
    ncb_callname: TNBNameRecord; (* name on local/remote adapter *)
    ncb_name: TNBNameRecord; (* name on local adapter *)
    ncb_rto: Byte; (* session recieve timeout *)
    ncb_sto: Byte; (* session send timeout *)
    ncb_post_offs,
      ncb_post_seg: Word; (* pointer to POST routine *)
    ncb_lana_num: Byte; (* adapter selection *)
    ncb_cmd_cplt: Byte; (* command complete status *)
    ncb_reserve: array[0..13] of Byte; (* reserved area *)
  end;

  { name buffer }
  TNBNameBuffer = packed record
    name: TNBNameRecord;
    name_num: Byte;
    name_flags: Byte;
  end;

  { mac address record }
  TNBMacAddress = packed array[0..5] of Byte;

  { this structure is used in an NCB_ADAPTERSTATUS netbios command }
  TNBAdapterStatus = packed record
    adapter_address: TNBMacAddress;
    rev_major: Byte;
    reserved0: Byte;
    adapter_type: Byte;
    rev_minor: Byte;
    duration: Word;
    frmr_recv: Word;
    frmr_xmit: Word;
    iframe_recv_err: Word;
    xmit_aborts: Word;
    xmit_success: LongInt;
    recv_success: LongInt;
    iframe_xmit_err: Word;
    recv_buff_unavail: Word;
    t1_timeouts: Word;
    ti_timeouts: Word;
    reserved1: LongInt;
    free_ncbs: Word;
    max_cfg_ncbs: Word;
    max_ncbs: Word;
    xmit_buf_unavail: Word;
    max_dgram_size: Word;
    pending_sess: Word;
    max_cfg_sess: Word;
    max_sess: Word;
    max_sess_pkt_size: Word;
    name_count: Word;
    Names: packed array[0..15] of TNBNameBuffer;
  end;

  { enumeration of adapters record }
  TLanaEnum = packed record
    length: Char; { Number of valid entries in lana[] }
    lana: array[0..MAX_LANA] of Char;
  end;

const
  { commands used to retrieve the adapter status }
  NCB_RESET = $32; { NCB RESET }
  NCB_ADAPTERSTATUS = $33; { NCB ADAPTER STATUS }
  NCB_ENUM = $37; { NCB ENUMERATE LANA NUMBERS }

  { the netbios call function }
function Netbios(var NCB: TNCB): Byte{$IFDEF WIN32}; stdcall{$ENDIF};

implementation
{$IFNDEF WIN32}
uses
  Dos; {Intr}

{ dos version of the netbios call (INT 5Ch) }

function Netbios(var NCB: TNCB): Byte;
var
  regs: Registers;
begin
  with regs do
  begin
    ES := seg(NCB);
    BX := ofs(NCB);
  end;
  Intr($5C, regs);
  Netbios := NCB.ncb_retcode;
end;
{$ELSE}

{ win32 version of the netbios call }
function Netbios; external 'netapi32.dll' name 'Netbios';
{$ENDIF}
end.
