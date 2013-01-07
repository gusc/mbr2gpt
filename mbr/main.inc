; Code location constants
%define ORG_LOC			0x7C00					; Initial MBR position in memory (where BIOS loads it)
%define RELOC_LOC		0x0800					; Relocation position (where we will copy neccessary MBR code to chainload bootloader)
%define	BUFF_LOC		0x0800					; Location of read buffer in memory
%define BOOT_LOC		0x7C00					; Location of BBP bootcode

; Local data structure
struc tDATA							
	.drive_id		resb	1					; drive ID
	._pad			resb	3					; dummy padding
	.bbp_size		resd	1					; BBP sector count
endstruc
%define DATA_LOC		0x0660					; Location of our global data structure in memory
%define DATA(x)  DATA_LOC + tDATA. %+ x			; Helper macro to clean up the code a bit

; MBR Partition entry structure
struc tMBRPart
	.status			resb	1					; Partition status (0x80 if bootable, 0x00 otherwise)
	.chs_start		resb	3					; Start of partition as a CHS value (we're not going to use this)
	.type			resb	1					; Partition type (we're looking for 0xEE)
	.chs_end		resb	3					; End of partition as a CHS value
	.lba_start		resd	1					; Start of partition as a LBA value
	.lba_length		resd	1					; Size of partition - number of sectors
endstruc
%define MBR_LOC			0x7DBE					; Partition table start
%define MBR(x)  MBR_LOC + tMBRPart. %+ x		; Helper macro to clean up the code a bit

; DAP structure for extended read
struc tDAP
	.size			resb	1					; Packet size
	.reserved		resb	1					; Reserved - should be 0
	.lba_count		resw	1					; Number of sectors to transfer
	.dest_buff		resd	1					; Desination buffer where to transfer data
	.lba_start_l	resd	1					; Low dword of start LBA
	.lba_start_h	resd	1					; High dword of start LBA
endstruc
%define DAP_LOC			0x0600					; Location in memory
%define DAP(x)	DAP_LOC + tDAP. %+ x			; Helper macro to clean up the code a bit

; GPT Header
struc tGPTHead
	.signature		resd	2					; EFI PART signature
	.version		resd	1					; Version number
	.header_size	resd	1					; Size of a header (should be 92)
	.header_crc32	resd	1					; CRC32 of first 3 entries
	.reserved		resd	1					; Should be 0
	.lba_curr		resd	2					; Current address of GPT header
	.lba_backup		resd	2					; Address of backup GPT header
	.lba_first		resd	2					; First usable LBA
	.lba_last		resd	2					; Last usable LBA
	.disk_guid		resd	4					; Disk GUID
	.part_arr_lba	resd	2					; LBA adress of partition array
	.part_count		resd	1					; Number of entries in partition array
	.part_size		resd	1					; Size of partition entry
	.part_arr_crc32	resd	1					; CRC32 of partition array
	; There should be a definition of 420 reserved bytes, bet we don't need them
endstruc
%define GPT(x)	BUFF_LOC + tGPTHead. %+ x		; Helper macro to clean up the code a bit

; GPT Partition
struc tGPTPart
	.part_guid		resd	4					; Partition type GUID
	.unique_guid	resd	4					; Unique GUID of partition
	.lba_start_l	resd	1					; Start LBA low dword
	.lba_start_h	resd	1					; Start LBA high dword
	.lba_end_l		resd	1					; End LBA low dword
	.lba_end_h		resd	1					; End LBA high dword
	.attributes		resd	2					; Partition attributes
	.name			resw	36					; UTF-16 partition name
endstruc
%define PART(x)	BUFF_LOC + tGPTPart. %+ x		; Helper macro to clean up the code a bit 
