
/*
 *  asm.h - definitions useful with inline assembly
 *
 *  Copyright (c) 1991 Symantec Corporation.  All rights reserved.
 *
 *  These interfaces are based on material copyrighted
 *  by Apple Computer, Inc., 1985-1991.
 *
 */

#ifndef __asm__
#define __asm__


/*  trap modifier flags, e.g. "NewHandle  SYS+CLEAR"  */

enum {
		/*  Memory Manager traps  */
	SYS = 2,			/*  applies to system heap  */
	CLEAR = 1,			/*  zero allocated block  */
		/*  File Manager and Device Manager traps  */
	ASYNC = 2,			/*  asynchronous I/O  */
	HFS = 1,			/*  HFS version of trap (File Manager)  */
	IMMED = 1,			/*  bypass driver queue (Device Manager)  */
		/*  string operations  */
	MARKS = 1,			/*  ignore diacritical marks  */
	CASE = 2,			/*  don't ignore case  */
		/*  GetTrapAddress, SetTrapAddress  */
	NEWOS = 1,			/*  new trap numbering, OS trap  */
	NEWTOOL = 3,		/*  new trap numbering, Toolbox trap  */
		/*  Toolbox traps  */
	AUTOPOP = 2			/*  return directly to caller's caller  */
};


/*  field offsets, e.g. "move.w  d0,OFFSET(Rect,bottom)(a2)"  */

#define OFFSET(type, field)		((int) &((type *) 0)->field)


/*
 *  additional trap definitions
 *
 *  Two kinds of traps are defined here.
 *
 *	*	Traps that may be useful in inline assembly, or that have
 *		traditionally been available in inline assembly in THINK C,
 *		but for which the Apple headers do not supply inline definitions.
 *
 *	*	Traps for which the Apple headers supply inline definitions
 *		that, though fine in C, are inappropriate for use in inline
 *		assembly.  (The inline assembler knows to prefer a trap defined
 *		with a leading underscore, regardless of whether an underscore
 *		is used when the trap is invoked.)
 *
 */

void _ADBOp(void) = 0xA07C;
void _AddDrive(void) = 0xA04E;

void _Chain(void) = 0xA9F3;
void _CmpString(void) = 0xA03C;

void _Date2Secs(void) = 0xA9C7;
void _DecStr68K(void) = 0xA9EE;
void _Delay(void) = 0xA03B;
void _DeleteUserIdentity(void) = { 0x700C, 0xA0DD };

void _Elems68K(void) = 0xA9EC;
void _EqualString(void) = 0xA03C;

void _FindFolder(void) = { 0x7000, 0xA823 };
void _FlushEvents(void) = 0xA032;
void _FP68K(void) = 0xA9EB;

void _Gestalt(void) = 0xA1AD;
void _GetDefaultUser(void) = { 0x700D, 0xA0DD };
void _GetHandleSize(void) = 0xA025;
void _GetItemStyle(void) = 0xA941;
void _GetOSEvent(void) = 0xA031;
void _GetPhysical(void) = { 0x7005, 0xA15C };
void _GetPtrSize(void) = 0xA021;

void _HandToHand(void) = 0xA9E1;
void _HCreateResFile(void) = 0xA81B;
void _HFSDispatch(void) = 0xA260;
void _HOpenResFile(void) = 0xA81A;

void _InitZone(void) = 0xA019;
void _InternalWait(void) = 0xA07F;

void _Launch(void) = 0xA9F2;
void _LwrString(void) = 0xA056;
void _LwrText(void) = 0xA056;

void _MaxMem(void) = 0xA11D;

void _NewGestalt(void) = 0xA3AD;
void _NMInstall(void) = 0xA05E;
void _NMRemove(void) = 0xA05F;

void _OSDispatch(void) = 0xA88F;
void _OSEventAvail(void) = 0xA030;

void _Pack0(void) = 0xA9E7;
void _Pack2(void) = 0xA9E9;
void _Pack3(void) = 0xA9EA;
void _Pack4(void) = 0xA9EB;
void _Pack5(void) = 0xA9EC;
void _Pack6(void) = 0xA9ED;
void _Pack7(void) = 0xA9EE;
void _Pack12(void) = 0xA82E;
void _PPostEvent(void) = 0xA12F;
void _PrGlue(void) = 0xA8FD;
void _PtrToHand(void) = 0xA9E3;
void _PurgeSpace(void) = 0xA162;

void _RelString(void) = 0xA050;
void _ReplaceGestalt(void) = 0xA5AD;

void _ScriptUtil(void) = 0xA8B5;
void _SCSIDispatch(void) = 0xA815;
void _SetFractEnable(void) = 0xA814;
void _ShutDown(void) = 0xA895;
void _SlotManager(void) = 0xA06E;
void _StartSecureSession(void) = { 0x700E, 0xA0DD };
void _StripAddress(void) = 0xA055;
void _SwapMMUMode(void) = 0xA05D;
void _SysEnvirons(void) = 0xA090;
void _SysError(void) = 0xA9C9;

void _TEDispatch(void) = 0xA83D;

void _UprString(void) = 0xA054;
void _UprText(void) = 0xA054;

void _WriteParam(void) = 0xA038;


#endif // __asm__
