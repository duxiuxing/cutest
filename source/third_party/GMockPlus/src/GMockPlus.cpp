#include "GMockPlus.h"
#include "HookMgr.h"
#include "MockMgr.h"

namespace testing {

typedef unsigned int uint;

DWORD AnalysisCallAddr(void* call_next_addr, DWORD* regs) {
	/*
	pushad ��ջ������:
	LOW
			edi
			esi
			ebp
			esp
			ebx
			edx
			ecx
			eax
	HIGH
	*/
	DWORD rEDI = regs[0];
	DWORD rESI = regs[1];
	DWORD rEBP = regs[2];
	DWORD rESP = regs[3] + 4;	// ����callָ���push eip��ջ�ϣ�����call���esp��callʱ��esp��4���ֽ�
	DWORD rEBX = regs[4];
	DWORD rEDX = regs[5];
	DWORD rECX = regs[6];
	DWORD rEAX = regs[7];

	char* binArr_2 = (char*)(byte*)call_next_addr - 2;
	char* binArr_3 = (char*)(byte*)call_next_addr - 3;
	char* binArr_5 = (char*)(byte*)call_next_addr - 5;
	char* binArr_6 = (char*)(byte*)call_next_addr - 6;

	// TODO: ����ڵ���״̬�¶�callָ��ĵط����˶ϵ㣬binArr[0]�ͳ�0xcc�ˣ�
	// TODO: ���ܻ����ͬʱ������ֹ�������
	// �磺e8 c5 34 ff 10
	// �ȱ�ʾ"CALL rel32	E8 xx xx xx xx"��
	// �ֱ�ʾ"CALL dword ptr [EAX]	FF 10"
	// ���������ʱû�д���
	DWORD callAddr = 0;
	byte bAddr;
	if (0 == callAddr && (byte)binArr_2[0] == 0xFF) {
		bAddr= binArr_2[1];
		switch(bAddr) {
		case 0x10:	// CALL dword ptr [EAX]	FF 10
			callAddr = *((DWORD*)rEAX);
			break;
		case 0x11:	// CALL dword ptr [ECX]	FF 11
			callAddr = *((DWORD*)rECX);
			break;
		case 0x12:	// CALL dword ptr [EDX]	FF 12
			callAddr = *((DWORD*)rEDX);
			break;
		case 0x13:	// CALL dword ptr [EBX]	FF 13
			callAddr = *((DWORD*)rEBX);
			break;
		case 0x16:	// CALL dword ptr [ESI]	FF 16
			callAddr = *((DWORD*)rESI);
			break;
		case 0x17:	// CALL dword ptr [EDI]	FF 17
			callAddr = *((DWORD*)rEDI);
			break;
		case 0xD0:	// CALL EAX	FF D0
			callAddr = rEAX;
			break;
		case 0xD1:	// CALL ECX	FF D1
			callAddr = rECX;
			break;
		case 0xD2:	// CALL EDX	FF D2
			callAddr = rEDX;
			break;
		case 0xD3:	// CALL EBX	FF D3
			callAddr = rEBX;
			break;
		case 0xD4:	// CALL ESP	FF D4
			callAddr = rESP;
			break;
		case 0xD5:	// CALL EBP	FF D5
			callAddr = rEBP;
			break;
		case 0xD6:	// CALL ESI	FF D6
			callAddr = rESI;
			break;
		case 0xD7:	// CALL EDI	FF D7
			callAddr = rEDI;
			break;
		default:
			break;
		}
	}
	if (0 == callAddr && (byte)binArr_3[0] == 0xFF) {
		bAddr = binArr_3[1];
		switch(bAddr) {
		case 0x50:	// CALL dword ptr [EAX+xx]	FF 50 xx
			callAddr = *((DWORD*)((byte*)rEAX + binArr_3[2]));
			break;
		case 0x51:	// CALL dword ptr [ECX+xx]	FF 51 xx
			callAddr = *((DWORD*)((byte*)rECX + binArr_3[2]));
			break;
		case 0x52:	// CALL dword ptr [EDX+xx]	FF 52 xx
			callAddr = *((DWORD*)((byte*)rEDX + binArr_3[2]));
			break;
		case 0x53:	// CALL dword ptr [EBX+xx]	FF 53 xx
			callAddr = *((DWORD*)((byte*)rEBX + binArr_3[2]));
			break;
		case 0x55:	// CALL dword ptr [EBP+xx]	FF 55 xx
			callAddr = *((DWORD*)((byte*)rEBP + binArr_3[2]));
			break;
		case 0x56:	// CALL dword ptr [ESI+xx]	FF 56 xx
			callAddr = *((DWORD*)((byte*)rESI + binArr_3[2]));
			break;
		case 0x57:	// CALL dword ptr [EDI+xx]	FF 57 xx
			callAddr = *((DWORD*)((byte*)rEDI + binArr_3[2]));
			break;
		default:
			break;
		}
	}
	if (0 == callAddr && (byte)binArr_6[0] == 0xFF) {
		bAddr = binArr_6[1];
		switch(bAddr) {
		case 0x15:	// CALL dword ptr [abs32]	FF 15 xx xx xx xx
			callAddr = **(DWORD**)(binArr_6 + 2);
			break;
		case 0x90:	// CALL dword ptr [EAX+xxxxxxxx]	FF 90 xx xx xx xx
			callAddr = *(DWORD*)(rEAX + *((DWORD*)(binArr_6 + 2)));
			break;
		case 0x91:	// CALL dword ptr [ECX+xxxxxxxx]	FF 91 xx xx xx xx
			callAddr = *(DWORD*)(rECX + *((DWORD*)(binArr_6 + 2)));
			break;
		case 0x92:	// CALL dword ptr [EDX+xxxxxxxx]	FF 92 xx xx xx xx
			callAddr = *(DWORD*)(rEDX + *((DWORD*)(binArr_6 + 2)));
			break;
		case 0x93:	// CALL dword ptr [EBX+xxxxxxxx]	FF 93 xx xx xx xx
			callAddr = *(DWORD*)(rEBX + *((DWORD*)(binArr_6 + 2)));
			break;
		case 0x95:	// CALL dword ptr [EBP+xxxxxxxx]	FF 95 xx xx xx xx
			callAddr = *(DWORD*)(rEBP + *((DWORD*)(binArr_6 + 2)));
			break;
		case 0x96:	// CALL dword ptr [ESI+xxxxxxxx]	FF 96 xx xx xx xx
			callAddr = *(DWORD*)(rESI + *((DWORD*)(binArr_6 + 2)));
			break;
		case 0x97:	// CALL dword ptr [EDI+xxxxxxxx]	FF 97 xx xx xx xx
			callAddr = *(DWORD*)(rEDI + *((DWORD*)(binArr_6 + 2)));
			break;
		default:
			break;
		}
	}
	if (0 == callAddr && (byte)binArr_5[0] == 0xE8) {
		// CALL rel32	E8 xx xx xx xx
		// ������ָ����жϷŵ������Ϊ���ָ��ֻ�ж� binArr_5[0] == 0xE8
		// �����׳������У���ǰ�����Щָ����ж������ֽڵ����ݵģ�����������
		callAddr = (DWORD)call_next_addr + *(DWORD*)(binArr_5 + 1);
	}
	/*
	��ʣ�����¼���ָ��û��֧�֣�
	ָ��	��������ʽ
	CALL dword ptr [REG*SCALE+BASE]	FF 14 xx

	CALL dword ptr [REG*SCALE+BASE+off8]	FF 54 xx xx
	CALL dword ptr [REG*SCALE+BASE+off32]	FF 94 xx xx xx xx xx

	CALL FAR seg16:abs32	9A xx xx xx xx xx xx
	*/

	/*
	NOTE:
	��������ᵼ�·���ֵcallAddrΪ0��
		Ҫmock�ĺ���û�д�������������ں������õĵط����˶ϵ㣻
	ԭ�������˶ϵ����������ڶϵ㴦�޸��ڴ�Ϊ0xCC���Ӷ����������޷����������������ֵ��÷�ʽ��
	�����취�ǣ��Ѷϵ�ȥ����
	*/

	return callAddr;
}

GMockPlus* QueryMockInfo(void* srcAddr) {
	GMockPlus* mockInfo = MockMgr::GetInstance()->QueryMockInfo(srcAddr);
	ASSERT(mockInfo);
	return mockInfo;
}

void HookAll() {
	HookMgr::GetInstance()->HookAll();
}

void UnhookAll() {
	HookMgr::GetInstance()->UnhookAll();
}

void* internal::QueryNewSrcAddrImpl_(void* srcAddr) {
	void* result = srcAddr;
	if (srcAddr) {
		GMockPlus* mockInfo = MockMgr::GetInstance()->QueryMockInfo(srcAddr);
		if (mockInfo) {
			result = mockInfo->GetNewSrcAddr();
		}
	}

	return result;
}

void __declspec(naked) MockFunctionJmpTo() {	// ��hook�ĺ���ֱ��jmp���ˣ��˺�����Ҫ����ջƽ�⣬ת���µ�ʵ��
	__asm {
		// 1.����Ĵ���
		pushad	// ������Ĵ�����һ��8���Ĵ�����
		push ebp
		mov ebp, esp

		sub esp, 16	// Ԥ���ĸ��ָ���ʱ����ʹ�ã�

		// 2.��ȡGMockPlus����ָ��
		// �ҵ��Ǵ�����jmp���˵�
		// ebp - 4 ���� srcFuncAddr
		mov ebx, ebp
		add ebx, 4
		push ebx	// ����ļĴ����ǵĵ�ַ
		mov ebx, [ebp + 36]
		push ebx
		call AnalysisCallAddr






		add esp, 8
		mov dword ptr [ebp - 4], eax

		// ͨ����call�ĺ�����ַ�ҵ�mock�����ָ��
		push eax
		call QueryMockInfo
		add esp, 4
		mov dword ptr [ebp - 4], eax	// ebp - 4 �ﱣ�����ָ��

		// 3.׼������������dstMethodAddr��dstMethodAddr����ĳ�Ա������ջƽ������dstMethodAddr��������
		mov ecx, dword ptr [ebp - 4]
		call GMockPlus::GetObjPtr
		mov dword ptr [ebp - 8], eax	// ebp - 8 �ﱣ�� objPtr
		mov ecx, dword ptr [ebp - 4]
		call GMockPlus::GetDstMethodAddr
		mov dword ptr [ebp - 12], eax	// ebp - 12 �ﱣ�� dstMethodAddr
		mov ecx, dword ptr [ebp - 4]
		call GMockPlus::GetParamCount
		mov dword ptr [ebp - 16], eax	// ebp - 16 �ﱣ�� paramCnt

		// call objPtr->dstMethodAddr
		call GMockPlus::ShouldPassParams	// hook�����Ĳ���������ԭ��������һ�£�Ҳ����û�в�������������Ĳ����Ļ���
		mov bl, al
		movzx eax, bl
		test eax, eax
		je callDstMethod	// ����Ҫ�������� objPtr->dstMethodAddr �Ļ���ֱ�ӵ���֮

		// ���򽫲���ѹ��ջ��
		mov ecx, dword ptr [ebp - 16]
		jmp compareWithZero
subParamCnt:
		sub ecx, 1
compareWithZero:
		cmp ecx, 0
		jbe callDstMethod	// �����Ѿ�push��
		mov eax, ecx
		shl eax, 2
		add eax, 4 + 32		// �����32��Ϊ������pushad�����8���Ĵ���
		mov ebx, dword ptr [ebp + eax]
		push ebx	// ������ջ
		jmp subParamCnt
callDstMethod:
		mov ecx, dword ptr [ebp - 8]	// [ebp - 8]�������objPtr��ָ��
		mov eax, dword ptr [ebp - 12]	// [ebp - 12]����������Ա������ָ��
		call eax
		mov dword ptr [ebp - 12], eax	// [ebp - 12] �ﱣ�淵��ֵ
		mov dword ptr [ebp - 8], edx	// �������ֵ��__int64�Ļ���edx��Ҳ�ᱣ�淵��ֵ

		// 4.ƽ��ջ��Ĵ����ָ�
		mov ecx, dword ptr [ebp - 4]
		call GMockPlus::IsCDeclCall		// ������� CDECL_CALL��ջƽ�⹤����Ҫ����������
		test al, al
		jne cdecl_call

		mov eax, dword ptr [ebp - 12]	// ���÷���ֵ�� eax
		mov edx, dword ptr [ebp - 8]	// ���÷���ֵ�� edx
		mov ecx, dword ptr [ebp - 16]	// [ebp - 16]�ﱣ����ǲ�����������������ջƽ��
		mov esp, ebp
		pop ebp
		jmp stack_balance

cdecl_call:	// cdecl ������Ϊ�ǲ�������Ϊ0�����
		mov eax, dword ptr [ebp - 12]	// ���÷���ֵ�� eax
		mov edx, dword ptr [ebp - 8]	// ���÷���ֵ�� edx
		mov ecx, 0
		mov esp, ebp
		pop ebp

stack_balance:
		push eax	// ���淵��ֵ��
		push edx	// ���淵��ֵ��
		/*
		��ʱ��ջ״̬��
		-------------- esp
		edx
		eax
		regs
		eip
		param_1
		...
		param_n
		*/

		mov edx, ecx
		shl edx, 2	// ����������ƫ��

		cmp ecx, 0
		je no_param_stack_balance

		mov ebx, dword ptr [esp + 32 + 8]	// ��һ��ָ��ĵ�ַ
		mov eax, esp
		add eax, 32 + 8
		add eax, edx
		mov dword ptr [eax], ebx		// ������һ��ָ��ĵ�ַ
		/*
		��ʱ��ջ״̬��
		-------------- esp
		edx
		eax
		regs
		eip
		param_1
		...
		param_n-1
		eip
		*/

		mov ecx, 8 + 2	// Ų�������8���Ĵ�����ջ����eax��edx������ֵ��
		mov esi, esp
		add esi, 8 + 32 - 4		// esiָ��pushad��ײ���reg
		mov edi, esi
		add edi, edx	// ediָ��Ҫ�����ĵط�
move_regs:
		cmp ecx, 0
		jbe no_param_stack_balance
		mov eax, dword ptr [esi]
		mov dword ptr [edi], eax
		sub esi, 4
		sub edi, 4
		sub ecx, 1
		jmp move_regs
no_param_stack_balance:
		/*
		��ʱ��ջ״̬��
		-------------- esp
		temp_1
		...
		temp_n
		edx
		eax
		regs
		eip
		*/
		add esp, edx
		add esp, 8
		/*
		��ʱ��ջ״̬��
		edx
		eax
		-------------- esp
		regs
		eip
		*/

		popad
		/*
		��ʱ��ջ״̬��
		edx
		eax
		regs
		-------------- esp
		eip
		*/
		mov eax, dword ptr [esp - 32 - 4]
		mov edx, dword ptr [esp - 32 - 8]
		ret
	}
}

GMockPlus::GMockPlus(void* srcAddr,
					   void* objPtr,
					   void* dstMethodAddr,
					   bool hookImmediately/* = true*/,
					   CallType callType /*= CDECL_CALL*/,
					   size_t paramCnt /*= 0*/,
					   bool passParams /*= true*/)
: m_callType(callType)
, m_paramCnt(paramCnt)
, m_srcAddr(srcAddr)
, m_dstAddr(NULL)
, m_objPtr(objPtr)
, m_dstMethodAddr(dstMethodAddr)
, m_newSrcAddr(NULL)
, m_hooked(false)
, m_registed(false)
, m_passParams(passParams) {
	ASSERT(srcAddr && objPtr && dstMethodAddr);
	m_dstAddr = MockFunctionJmpTo;
	PreHook();
	if (hookImmediately) {
		Hook();
	}
}

GMockPlus::GMockPlus(void* srcAddr, void* dstAddr, bool hookImmediately/* = true*/)
:m_callType(INVALID_CALL)
,m_paramCnt(0)
,m_srcAddr(srcAddr)
,m_dstAddr(dstAddr)
,m_objPtr(NULL)
,m_dstMethodAddr(NULL)
,m_newSrcAddr(NULL)
,m_hooked(false)
,m_registed(false)
,m_passParams(false) {
	ASSERT(srcAddr && dstAddr);
	PreHook();
	if (hookImmediately) {
		Hook();
	}
}

GMockPlus::~GMockPlus() {
	Unhook();
}

void GMockPlus::PreHook() {
	if (!m_registed && !m_hooked) {
		HookMgr::GetInstance()->AddFuncHook(this);
		m_registed = true;
	}
}

void GMockPlus::Hook() {
	if (!m_registed && !m_hooked) {	// !m_hooked ���ڷ�ֹPreUnhook���ֵ���Mock
		HookMgr::GetInstance()->AddFuncHook(this);
		m_registed = true;
	}
	HookMgr::GetInstance()->HookAll();
	ASSERT(m_hooked);
}

void GMockPlus::PreUnhook() {
	if (m_registed && m_hooked) {
		HookMgr::GetInstance()->AddFuncUnhook(this);
		m_registed = false;
	}
}

void GMockPlus::Unhook() {
	if (m_registed && m_hooked) {	// m_hooked ���ڷ�ֹ�������ɺ�û����Mock����MockAll�͵�����Unmock
		HookMgr::GetInstance()->AddFuncUnhook(this);
		m_registed = false;
	}
	HookMgr::GetInstance()->UnhookAll();
	// assert(!m_hooked);
}

}