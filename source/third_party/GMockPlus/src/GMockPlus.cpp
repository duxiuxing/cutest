#include "GMockPlus.h"
#include "HookMgr.h"
#include "MockMgr.h"

namespace testing {

typedef unsigned int uint;

DWORD AnalysisCallAddr(void* call_next_addr, DWORD* regs) {
  /*
    pushad 后栈的内容:
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
  DWORD rESP = regs[3] + 4; // 由于call指令会push eip到栈上，所以call后的esp比call时的esp低4个字节
  DWORD rEBX = regs[4];
  DWORD rEDX = regs[5];
  DWORD rECX = regs[6];
  DWORD rEAX = regs[7];

  char* binArr_2 = (char*)(byte*)call_next_addr - 2;
  char* binArr_3 = (char*)(byte*)call_next_addr - 3;
  char* binArr_5 = (char*)(byte*)call_next_addr - 5;
  char* binArr_6 = (char*)(byte*)call_next_addr - 6;

  // TODO: 如果在调试状态下对call指令的地方下了断点，binArr[0]就成0xcc了！
  // TODO: 可能会出现同时满足多种规则的情况
  // 如：e8 c5 34 ff 10
  // 既表示"CALL rel32  E8 xx xx xx xx"，
  // 又表示"CALL dword ptr [EAX]  FF 10"
  // 这种情况暂时没有处理
  DWORD callAddr = 0;
  byte bAddr;
  if (0 == callAddr && (byte)binArr_2[0] == 0xFF) {
    bAddr = binArr_2[1];
    switch (bAddr) {
    case 0x10:  // CALL dword ptr [EAX] FF 10
      callAddr = *((DWORD*)rEAX);
      break;
    case 0x11:  // CALL dword ptr [ECX] FF 11
      callAddr = *((DWORD*)rECX);
      break;
    case 0x12:  // CALL dword ptr [EDX] FF 12
      callAddr = *((DWORD*)rEDX);
      break;
    case 0x13:  // CALL dword ptr [EBX] FF 13
      callAddr = *((DWORD*)rEBX);
      break;
    case 0x16:  // CALL dword ptr [ESI] FF 16
      callAddr = *((DWORD*)rESI);
      break;
    case 0x17:  // CALL dword ptr [EDI] FF 17
      callAddr = *((DWORD*)rEDI);
      break;
    case 0xD0:  // CALL EAX FF D0
      callAddr = rEAX;
      break;
    case 0xD1:  // CALL ECX FF D1
      callAddr = rECX;
      break;
    case 0xD2:  // CALL EDX FF D2
      callAddr = rEDX;
      break;
    case 0xD3:  // CALL EBX FF D3
      callAddr = rEBX;
      break;
    case 0xD4:  // CALL ESP FF D4
      callAddr = rESP;
      break;
    case 0xD5:  // CALL EBP FF D5
      callAddr = rEBP;
      break;
    case 0xD6:  // CALL ESI FF D6
      callAddr = rESI;
      break;
    case 0xD7:  // CALL EDI FF D7
      callAddr = rEDI;
      break;
    default:
      break;
    }
  }
  if (0 == callAddr && (byte)binArr_3[0] == 0xFF) {
    bAddr = binArr_3[1];
    switch (bAddr) {
    case 0x50:  // CALL dword ptr [EAX+xx]  FF 50 xx
      callAddr = *((DWORD*)((byte*)rEAX + binArr_3[2]));
      break;
    case 0x51:  // CALL dword ptr [ECX+xx]  FF 51 xx
      callAddr = *((DWORD*)((byte*)rECX + binArr_3[2]));
      break;
    case 0x52:  // CALL dword ptr [EDX+xx]  FF 52 xx
      callAddr = *((DWORD*)((byte*)rEDX + binArr_3[2]));
      break;
    case 0x53:  // CALL dword ptr [EBX+xx]  FF 53 xx
      callAddr = *((DWORD*)((byte*)rEBX + binArr_3[2]));
      break;
    case 0x55:  // CALL dword ptr [EBP+xx]  FF 55 xx
      callAddr = *((DWORD*)((byte*)rEBP + binArr_3[2]));
      break;
    case 0x56:  // CALL dword ptr [ESI+xx]  FF 56 xx
      callAddr = *((DWORD*)((byte*)rESI + binArr_3[2]));
      break;
    case 0x57:  // CALL dword ptr [EDI+xx]  FF 57 xx
      callAddr = *((DWORD*)((byte*)rEDI + binArr_3[2]));
      break;
    default:
      break;
    }
  }
  if (0 == callAddr && (byte)binArr_6[0] == 0xFF) {
    bAddr = binArr_6[1];
    switch (bAddr) {
    case 0x15:  // CALL dword ptr [abs32] FF 15 xx xx xx xx
      callAddr = **(DWORD**)(binArr_6 + 2);
      break;
    case 0x90:  // CALL dword ptr [EAX+xxxxxxxx]  FF 90 xx xx xx xx
      callAddr = *(DWORD*)(rEAX + * ((DWORD*)(binArr_6 + 2)));
      break;
    case 0x91:  // CALL dword ptr [ECX+xxxxxxxx]  FF 91 xx xx xx xx
      callAddr = *(DWORD*)(rECX + * ((DWORD*)(binArr_6 + 2)));
      break;
    case 0x92:  // CALL dword ptr [EDX+xxxxxxxx]  FF 92 xx xx xx xx
      callAddr = *(DWORD*)(rEDX + * ((DWORD*)(binArr_6 + 2)));
      break;
    case 0x93:  // CALL dword ptr [EBX+xxxxxxxx]  FF 93 xx xx xx xx
      callAddr = *(DWORD*)(rEBX + * ((DWORD*)(binArr_6 + 2)));
      break;
    case 0x95:  // CALL dword ptr [EBP+xxxxxxxx]  FF 95 xx xx xx xx
      callAddr = *(DWORD*)(rEBP + * ((DWORD*)(binArr_6 + 2)));
      break;
    case 0x96:  // CALL dword ptr [ESI+xxxxxxxx]  FF 96 xx xx xx xx
      callAddr = *(DWORD*)(rESI + * ((DWORD*)(binArr_6 + 2)));
      break;
    case 0x97:  // CALL dword ptr [EDI+xxxxxxxx]  FF 97 xx xx xx xx
      callAddr = *(DWORD*)(rEDI + * ((DWORD*)(binArr_6 + 2)));
      break;
    default:
      break;
    }
  }
  if (0 == callAddr && (byte)binArr_5[0] == 0xE8) {
    // CALL rel32 E8 xx xx xx xx
    // 把这种指令的判断放到最后，因为这个指令只判断 binArr_5[0] == 0xE8
    // 更容易出现误判，而前面的那些指令都是判断两个字节的内容的，不容易误判
    callAddr = (DWORD)call_next_addr + *(DWORD*)(binArr_5 + 1);
  }
  /*
    还剩下以下几种指令没有支持：
    指令  二进制形式
    CALL dword ptr [REG*SCALE+BASE] FF 14 xx

    CALL dword ptr [REG*SCALE+BASE+off8]  FF 54 xx xx
    CALL dword ptr [REG*SCALE+BASE+off32] FF 94 xx xx xx xx xx

    CALL FAR seg16:abs32  9A xx xx xx xx xx xx
  */

  /*
    NOTE:
    以下情况会导致返回值callAddr为0：
    要mock的函数没有传入参数，并且在函数调用的地方下了断点；
    原因是下了断点后调试器会在断点处修改内存为0xCC，从而导致这里无法解析出到底是哪种调用方式，
    修正办法是：把断点去掉。
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

void __declspec(naked) MockFunctionJmpTo() {  // 被hook的函数直接jmp到此，此函数需要负责栈平衡，转调新的实现
  __asm {
    // 1.保存寄存器
    pushad  // 保存各寄存器（一共8个寄存器）
    push ebp
    mov ebp, esp

    sub esp, 16 // 预留四个字给临时变量使用！

    // 2.获取GMockPlus对象指针
    // 找到是从哪里jmp到此的
    // ebp - 4 保存 srcFuncAddr
    mov ebx, ebp
    add ebx, 4
    push ebx  // 保存的寄存器们的地址
    mov ebx, [ebp + 36]
    push ebx
    call AnalysisCallAddr

    add esp, 8
    mov dword ptr [ebp - 4], eax

    // 通过所call的函数地址找到mock对象的指针
    push eax
    call QueryMockInfo
    add esp, 4
    mov dword ptr [ebp - 4], eax  // ebp - 4 里保存对象指针

    // 3.准备参数并调用dstMethodAddr，dstMethodAddr是类的成员函数，栈平衡是由dstMethodAddr函数做的
    mov ecx, dword ptr [ebp - 4]
    call GMockPlus::GetObjPtr
    mov dword ptr [ebp - 8], eax  // ebp - 8 里保存 objPtr
    mov ecx, dword ptr [ebp - 4]
    call GMockPlus::GetDstMethodAddr
    mov dword ptr [ebp - 12], eax // ebp - 12 里保存 dstMethodAddr
    mov ecx, dword ptr [ebp - 4]
    call GMockPlus::GetParamCount
    mov dword ptr [ebp - 16], eax // ebp - 16 里保存 paramCnt

    // call objPtr->dstMethodAddr
    call GMockPlus::ShouldPassParams  // hook函数的参数可以与原函数保持一致，也可以没有参数（如果不关心参数的话）
    mov bl, al
    movzx eax, bl
    test eax, eax
    je callDstMethod  // 不需要传参数给 objPtr->dstMethodAddr 的话就直接调用之

    // 否则将参数压到栈上
    mov ecx, dword ptr [ebp - 16]
    jmp compareWithZero
    subParamCnt:
    sub ecx, 1
    compareWithZero:
    cmp ecx, 0
    jbe callDstMethod // 参数已经push完
    mov eax, ecx
    shl eax, 2
    add eax, 4 + 32   // 这里的32是为了跳过pushad保存的8个寄存器
    mov ebx, dword ptr [ebp + eax]
    push ebx  // 参数入栈
    jmp subParamCnt
    callDstMethod:
    mov ecx, dword ptr [ebp - 8]  // [ebp - 8]保存的是objPtr的指针
    mov eax, dword ptr [ebp - 12] // [ebp - 12]保存的是类成员函数的指针
    call eax
    mov dword ptr [ebp - 12], eax // [ebp - 12] 里保存返回值
    mov dword ptr [ebp - 8], edx  // 如果返回值是__int64的话，edx中也会保存返回值

    // 4.平衡栈与寄存器恢复
    mov ecx, dword ptr [ebp - 4]
    call GMockPlus::IsCDeclCall   // 如果不是 CDECL_CALL，栈平衡工作需要放在这里做
    test al, al
    jne cdecl_call

    mov eax, dword ptr [ebp - 12] // 设置返回值到 eax
    mov edx, dword ptr [ebp - 8]  // 设置返回值到 edx
    mov ecx, dword ptr [ebp - 16] // [ebp - 16]里保存的是参数个数，这里做下栈平衡
    mov esp, ebp
    pop ebp
    jmp stack_balance

    cdecl_call: // cdecl 可以认为是参数个数为0的情况
    mov eax, dword ptr [ebp - 12] // 设置返回值到 eax
    mov edx, dword ptr [ebp - 8]  // 设置返回值到 edx
    mov ecx, 0
    mov esp, ebp
    pop ebp

    stack_balance:
    push eax  // 保存返回值！
    push edx  // 保存返回值！
    /*
      此时的栈状态：
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
    shl edx, 2  // 参数个数的偏移

    cmp ecx, 0
    je no_param_stack_balance

    mov ebx, dword ptr [esp + 32 + 8] // 下一条指令的地址
    mov eax, esp
    add eax, 32 + 8
    add eax, edx
    mov dword ptr [eax], ebx    // 设置下一条指令的地址
    /*
      此时的栈状态：
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

    mov ecx, 8 + 2  // 挪动缓存的8个寄存器和栈顶的eax和edx（返回值）
    mov esi, esp
    add esi, 8 + 32 - 4   // esi指向pushad最底部的reg
    mov edi, esi
    add edi, edx  // edi指向要移往的地方
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
      此时的栈状态：
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
      此时的栈状态：
      edx
      eax
      -------------- esp
      regs
      eip
    */

    popad
    /*
      此时的栈状态：
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

GMockPlus::GMockPlus(
  void* srcAddr,
  void* objPtr,
  void* dstMethodAddr,
  bool hookImmediately/* = true*/,
  CallType callType /*= CDECL_CALL*/,
  size_t paramCnt /*= 0*/,
  bool passParams /*= true*/
) : m_callType(callType)
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
  : m_callType(INVALID_CALL)
  , m_paramCnt(0)
  , m_srcAddr(srcAddr)
  , m_dstAddr(dstAddr)
  , m_objPtr(NULL)
  , m_dstMethodAddr(NULL)
  , m_newSrcAddr(NULL)
  , m_hooked(false)
  , m_registed(false)
  , m_passParams(false) {
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
  if (!m_registed && !m_hooked) { // !m_hooked 用于防止PreUnhook后又调用Mock
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
  if (m_registed && m_hooked) { // m_hooked 用于防止对象生成后还没调用Mock或者MockAll就调用了Unmock
    HookMgr::GetInstance()->AddFuncUnhook(this);
    m_registed = false;
  }
  HookMgr::GetInstance()->UnhookAll();
  // assert(!m_hooked);
}

} // namespace testing {
