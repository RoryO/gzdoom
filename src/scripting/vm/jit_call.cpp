
#include "jitintern.h"

void JitCompiler::EmitPARAM()
{
	using namespace asmjit;

	int index = NumParam++;
	ParamOpcodes.Push(pc);

	X86Gp stackPtr, tmp;
	X86Xmm tmp2;

	switch (B)
	{
	case REGT_NIL:
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, a)), (int64_t)0);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_NIL);
		break;
	case REGT_INT:
		cc.mov(x86::dword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, i)), regD[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_INT);
		break;
	case REGT_INT | REGT_ADDROF:
		stackPtr = newTempIntPtr();
		cc.mov(stackPtr, frameD);
		cc.add(stackPtr, (int)(C * sizeof(int32_t)));
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, a)), stackPtr);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_POINTER);
		break;
	case REGT_INT | REGT_KONST:
		cc.mov(x86::dword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, i)), konstd[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_INT);
		break;
	case REGT_STRING:
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, sp)), regS[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_STRING);
		break;
	case REGT_STRING | REGT_ADDROF:
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, a)), regS[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_POINTER);
		break;
	case REGT_STRING | REGT_KONST:
		tmp = newTempIntPtr();
		cc.mov(tmp, asmjit::imm_ptr(&konsts[C]));
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, sp)), tmp);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_STRING);
		break;
	case REGT_POINTER:
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, a)), regA[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_POINTER);
		break;
	case REGT_POINTER | REGT_ADDROF:
		stackPtr = newTempIntPtr();
		cc.mov(stackPtr, frameA);
		cc.add(stackPtr, (int)(C * sizeof(void*)));
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, a)), stackPtr);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_POINTER);
		break;
	case REGT_POINTER | REGT_KONST:
		tmp = newTempIntPtr();
		cc.mov(tmp, asmjit::imm_ptr(konsta[C].v));
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, a)), tmp);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_POINTER);
		break;
	case REGT_FLOAT:
		cc.movsd(x86::qword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, f)), regF[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_FLOAT);
		break;
	case REGT_FLOAT | REGT_MULTIREG2:
		cc.movsd(x86::qword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, f)), regF[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_FLOAT);
		index = NumParam++;
		ParamOpcodes.Push(pc);
		cc.movsd(x86::qword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, f)), regF[C + 1]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_FLOAT);
		break;
	case REGT_FLOAT | REGT_MULTIREG3:
		cc.movsd(x86::qword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, f)), regF[C]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_FLOAT);
		index = NumParam++;
		ParamOpcodes.Push(pc);
		cc.movsd(x86::qword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, f)), regF[C + 1]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_FLOAT);
		index = NumParam++;
		ParamOpcodes.Push(pc);
		cc.movsd(x86::qword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, f)), regF[C + 2]);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_FLOAT);
		break;
	case REGT_FLOAT | REGT_ADDROF:
		stackPtr = newTempIntPtr();
		cc.mov(stackPtr, frameF);
		cc.add(stackPtr, (int)(C * sizeof(double)));
		cc.mov(x86::ptr(params, index * sizeof(VMValue) + offsetof(VMValue, a)), stackPtr);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_POINTER);
		break;
	case REGT_FLOAT | REGT_KONST:
		tmp = newTempIntPtr();
		tmp2 = newTempXmmSd();
		cc.mov(tmp, asmjit::imm_ptr(konstf + C));
		cc.movsd(tmp2, asmjit::x86::qword_ptr(tmp));
		cc.movsd(x86::qword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, f)), tmp2);
		cc.mov(x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_FLOAT);
		break;
	default:
		I_FatalError("Unknown REGT value passed to EmitPARAM\n");
		break;
	}

}

void JitCompiler::EmitPARAMI()
{
	int index = NumParam++;
	ParamOpcodes.Push(pc);
	cc.mov(asmjit::x86::dword_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, i)), (int)ABCs);
	cc.mov(asmjit::x86::byte_ptr(params, index * sizeof(VMValue) + offsetof(VMValue, Type)), (int)REGT_INT);
}

void JitCompiler::EmitRESULT()
{
	// This instruction is just a placeholder to indicate where a return
	// value should be stored. It does nothing on its own and should not
	// be executed.
}

void JitCompiler::EmitCALL()
{
	EmitDoCall(regA[A], CallType::Unknown);
}

void JitCompiler::EmitCALL_K()
{
	VMFunction *func = (VMFunction*)konsta[A].o;

	auto ptr = newTempIntPtr();
	cc.mov(ptr, asmjit::imm_ptr(func));
	EmitDoCall(ptr, (func->VarFlags & VARF_Native) ? CallType::Native : CallType::Script);
}

void JitCompiler::EmitTAIL()
{
	EmitDoTail(regA[A], CallType::Unknown);
}

void JitCompiler::EmitTAIL_K()
{
	VMFunction *func = (VMFunction*)konsta[A].o;

	auto ptr = newTempIntPtr();
	cc.mov(ptr, asmjit::imm_ptr(func));
	EmitDoTail(ptr, (func->VarFlags & VARF_Native) ? CallType::Native : CallType::Script);
}

void JitCompiler::EmitDoCall(asmjit::X86Gp vmfunc, CallType calltype)
{
	using namespace asmjit;

	if (NumParam < B)
		I_FatalError("OP_CALL parameter count does not match the number of preceding OP_PARAM instructions");

	StoreInOuts(B);
	FillReturns(pc + 1, C);

	X86Gp paramsptr;
	if (B != NumParam)
	{
		paramsptr = newTempIntPtr();
		cc.lea(paramsptr, x86::ptr(params, (int)((NumParam - B) * sizeof(VMValue))));
	}
	else
	{
		paramsptr = params;
	}

	if (calltype == CallType::Script)
	{
		EmitScriptCall(vmfunc, paramsptr);
	}
	else if (calltype == CallType::Native)
	{
		EmitNativeCall(vmfunc, paramsptr);
	}
	else
	{
		auto nativecall = cc.newLabel();
		auto endcall = cc.newLabel();
		auto varflags = newTempInt32();
		cc.mov(varflags, x86::dword_ptr(vmfunc, offsetof(VMFunction, VarFlags)));
		cc.test(varflags, (int)VARF_Native);
		cc.jnz(nativecall);
		EmitScriptCall(vmfunc, paramsptr);
		cc.jmp(endcall);
		cc.bind(nativecall);
		EmitNativeCall(vmfunc, paramsptr);
		cc.bind(endcall);
	}

	LoadInOuts(B);
	LoadReturns(pc + 1, C);

	NumParam -= B;
	ParamOpcodes.Resize(ParamOpcodes.Size() - B);
}

void JitCompiler::EmitScriptCall(asmjit::X86Gp vmfunc, asmjit::X86Gp paramsptr)
{
	using namespace asmjit;

	// VMCalls[0]++
	auto vmcallsptr = newTempIntPtr();
	auto vmcalls = newTempInt32();
	cc.mov(vmcallsptr, imm_ptr(VMCalls));
	cc.mov(vmcalls, x86::dword_ptr(vmcallsptr));
	cc.add(vmcalls, (int)1);
	cc.mov(x86::dword_ptr(vmcallsptr), vmcalls);

	auto scriptcall = newTempIntPtr();
	cc.mov(scriptcall, x86::ptr(vmfunc, offsetof(VMScriptFunction, ScriptCall)));

	auto result = newResultInt32();
	auto call = cc.call(scriptcall, FuncSignature5<int, VMFunction *, VMValue*, int, VMReturn*, int>());
	call->setRet(0, result);
	call->setArg(0, vmfunc);
	call->setArg(1, paramsptr);
	call->setArg(2, Imm(B));
	call->setArg(3, callReturns);
	call->setArg(4, Imm(C));
}

void JitCompiler::EmitNativeCall(asmjit::X86Gp vmfunc, asmjit::X86Gp paramsptr)
{
	using namespace asmjit;
	auto result = newResultInt32();
	auto call = CreateCall<int, VMFunction*, int, int, VMValue*, VMReturn*>(&JitCompiler::DoNativeCall);
	call->setRet(0, result);
	call->setArg(0, vmfunc);
	call->setArg(1, Imm(B));
	call->setArg(2, Imm(C));
	call->setArg(3, paramsptr);
	call->setArg(4, callReturns);
}

void JitCompiler::EmitDoTail(asmjit::X86Gp vmfunc, CallType calltype)
{
	// Whereas the CALL instruction uses its third operand to specify how many return values
	// it expects, TAIL ignores its third operand and uses whatever was passed to this Exec call.

	// Note: this is not a true tail call, but then again, it isn't in the vmexec implementation either..

	using namespace asmjit;

	if (NumParam < B)
		I_FatalError("OP_TAIL parameter count does not match the number of preceding OP_PARAM instructions");

	StoreInOuts(B); // Is REGT_ADDROF even allowed for (true) tail calls?

	X86Gp paramsptr;
	if (B != NumParam)
	{
		paramsptr = newTempIntPtr();
		cc.lea(paramsptr, x86::ptr(params, (int)((NumParam - B) * sizeof(VMValue))));
	}
	else
	{
		paramsptr = params;
	}

	auto result = newResultInt32();

	if (calltype == CallType::Script)
	{
		EmitScriptTailCall(vmfunc, result, paramsptr);
	}
	else if (calltype == CallType::Native)
	{
		EmitNativeTailCall(vmfunc, result, paramsptr);
	}
	else
	{
		auto nativecall = cc.newLabel();
		auto endcall = cc.newLabel();
		auto varflags = newTempInt32();
		cc.mov(varflags, x86::dword_ptr(vmfunc, offsetof(VMFunction, VarFlags)));
		cc.test(varflags, (int)VARF_Native);
		cc.jnz(nativecall);
		EmitScriptTailCall(vmfunc, result, paramsptr);
		cc.jmp(endcall);
		cc.bind(nativecall);
		EmitNativeTailCall(vmfunc, result, paramsptr);
		cc.bind(endcall);
	}

	EmitPopFrame();
	cc.ret(result);

	NumParam -= B;
	ParamOpcodes.Resize(ParamOpcodes.Size() - B);
}

void JitCompiler::EmitScriptTailCall(asmjit::X86Gp vmfunc, asmjit::X86Gp result, asmjit::X86Gp paramsptr)
{
	using namespace asmjit;

	// VMCalls[0]++
	auto vmcallsptr = newTempIntPtr();
	auto vmcalls = newTempInt32();
	cc.mov(vmcallsptr, imm_ptr(VMCalls));
	cc.mov(vmcalls, x86::dword_ptr(vmcallsptr));
	cc.add(vmcalls, (int)1);
	cc.mov(x86::dword_ptr(vmcallsptr), vmcalls);

	auto scriptcall = newTempIntPtr();
	cc.mov(scriptcall, x86::ptr(vmfunc, offsetof(VMScriptFunction, ScriptCall)));

	auto call = cc.call(scriptcall, FuncSignature5<int, VMFunction *, VMValue*, int, VMReturn*, int>());
	call->setRet(0, result);
	call->setArg(0, vmfunc);
	call->setArg(1, paramsptr);
	call->setArg(2, Imm(B));
	call->setArg(3, ret);
	call->setArg(4, numret);
}

void JitCompiler::EmitNativeTailCall(asmjit::X86Gp vmfunc, asmjit::X86Gp result, asmjit::X86Gp paramsptr)
{
	using namespace asmjit;

	auto call = CreateCall<int, VMFunction*, int, int, VMValue*, VMReturn*>(&JitCompiler::DoNativeCall);
	call->setRet(0, result);
	call->setArg(0, vmfunc);
	call->setArg(1, Imm(B));
	call->setArg(2, numret);
	call->setArg(3, paramsptr);
	call->setArg(4, ret);
}

void JitCompiler::StoreInOuts(int b)
{
	using namespace asmjit;

	for (unsigned int i = ParamOpcodes.Size() - b; i < ParamOpcodes.Size(); i++)
	{
		asmjit::X86Gp stackPtr;
		auto c = ParamOpcodes[i]->c;
		switch (ParamOpcodes[i]->b)
		{
		case REGT_INT | REGT_ADDROF:
			stackPtr = newTempIntPtr();
			cc.mov(stackPtr, frameD);
			cc.add(stackPtr, (int)(c * sizeof(int32_t)));
			cc.mov(x86::dword_ptr(stackPtr), regD[c]);
			break;
		case REGT_STRING | REGT_ADDROF:
			// We don't have to do anything in this case. String values are never moved to virtual registers.
			break;
		case REGT_POINTER | REGT_ADDROF:
			stackPtr = newTempIntPtr();
			cc.mov(stackPtr, frameA);
			cc.add(stackPtr, (int)(c * sizeof(void*)));
			cc.mov(x86::ptr(stackPtr), regA[c]);
			break;
		case REGT_FLOAT | REGT_ADDROF:
			stackPtr = newTempIntPtr();
			cc.mov(stackPtr, frameF);
			cc.add(stackPtr, (int)(c * sizeof(double)));
			cc.movsd(x86::qword_ptr(stackPtr), regF[c]);

			// When passing the address to a float we don't know if the receiving function will treat it as float, vec2 or vec3.
			if ((unsigned int)c + 1 < regF.Size())
			{
				cc.add(stackPtr, (int)sizeof(double));
				cc.movsd(x86::qword_ptr(stackPtr), regF[c + 1]);
			}
			if ((unsigned int)c + 2 < regF.Size())
			{
				cc.add(stackPtr, (int)sizeof(double));
				cc.movsd(x86::qword_ptr(stackPtr), regF[c + 2]);
			}
			break;
		default:
			break;
		}
	}
}

void JitCompiler::LoadInOuts(int b)
{
	for (unsigned int i = ParamOpcodes.Size() - b; i < ParamOpcodes.Size(); i++)
	{
		const VMOP &param = *ParamOpcodes[i];
		if (param.op == OP_PARAM && (param.b & REGT_ADDROF))
		{
			LoadCallResult(param, true);
		}
	}
}

void JitCompiler::LoadReturns(const VMOP *retval, int numret)
{
	for (int i = 0; i < numret; ++i)
	{
		if (retval[i].op != OP_RESULT)
			I_FatalError("Expected OP_RESULT to follow OP_CALL\n");

		LoadCallResult(retval[i], false);
	}
}

void JitCompiler::LoadCallResult(const VMOP &opdata, bool addrof)
{
	int type = opdata.b;
	int regnum = opdata.c;

	switch (type & REGT_TYPE)
	{
	case REGT_INT:
		cc.mov(regD[regnum], asmjit::x86::dword_ptr(frameD, regnum * sizeof(int32_t)));
		break;
	case REGT_FLOAT:
		cc.movsd(regF[regnum], asmjit::x86::qword_ptr(frameF, regnum * sizeof(double)));
		if (addrof)
		{
			// When passing the address to a float we don't know if the receiving function will treat it as float, vec2 or vec3.
			if ((unsigned int)regnum + 1 < regF.Size())
				cc.movsd(regF[regnum + 1], asmjit::x86::qword_ptr(frameF, (regnum + 1) * sizeof(double)));
			if ((unsigned int)regnum + 2 < regF.Size())
				cc.movsd(regF[regnum + 2], asmjit::x86::qword_ptr(frameF, (regnum + 2) * sizeof(double)));
		}
		else if (type & REGT_MULTIREG2)
		{
			cc.movsd(regF[regnum + 1], asmjit::x86::qword_ptr(frameF, (regnum + 1) * sizeof(double)));
		}
		else if (type & REGT_MULTIREG3)
		{
			cc.movsd(regF[regnum + 1], asmjit::x86::qword_ptr(frameF, (regnum + 1) * sizeof(double)));
			cc.movsd(regF[regnum + 2], asmjit::x86::qword_ptr(frameF, (regnum + 2) * sizeof(double)));
		}
		break;
	case REGT_STRING:
		// We don't have to do anything in this case. String values are never moved to virtual registers.
		break;
	case REGT_POINTER:
		cc.mov(regA[regnum], asmjit::x86::ptr(frameA, regnum * sizeof(void*)));
		break;
	default:
		I_FatalError("Unknown OP_RESULT/OP_PARAM type encountered in LoadCallResult\n");
		break;
	}
}

void JitCompiler::FillReturns(const VMOP *retval, int numret)
{
	using namespace asmjit;

	for (int i = 0; i < numret; ++i)
	{
		if (retval[i].op != OP_RESULT)
		{
			I_FatalError("Expected OP_RESULT to follow OP_CALL\n");
		}

		int type = retval[i].b;
		int regnum = retval[i].c;

		if (type & REGT_KONST)
		{
			I_FatalError("OP_RESULT with REGT_KONST is not allowed\n");
		}

		auto regPtr = newTempIntPtr();

		switch (type & REGT_TYPE)
		{
		case REGT_INT:
			cc.mov(regPtr, frameD);
			cc.add(regPtr, (int)(regnum * sizeof(int32_t)));
			break;
		case REGT_FLOAT:
			cc.mov(regPtr, frameF);
			cc.add(regPtr, (int)(regnum * sizeof(double)));
			break;
		case REGT_STRING:
			cc.mov(regPtr, frameS);
			cc.add(regPtr, (int)(regnum * sizeof(FString)));
			break;
		case REGT_POINTER:
			cc.mov(regPtr, frameA);
			cc.add(regPtr, (int)(regnum * sizeof(void*)));
			break;
		default:
			I_FatalError("Unknown OP_RESULT type encountered in FillReturns\n");
			break;
		}

		cc.mov(x86::ptr(callReturns, i * sizeof(VMReturn) + offsetof(VMReturn, Location)), regPtr);
		cc.mov(x86::byte_ptr(callReturns, i * sizeof(VMReturn) + offsetof(VMReturn, RegType)), type);
	}
}

int JitCompiler::DoNativeCall(VMFunction *call, int b, int c, VMValue *param, VMReturn *returns)
{
	try
	{
		assert((call->VarFlags & VARF_Native) && "DoNativeCall must only be called for native functions");

		VMCycles[0].Unclock();
		int numret = static_cast<VMNativeFunction *>(call)->NativeCall(param, call->DefaultArgs, b, returns, c);
		VMCycles[0].Clock();

		return numret;
	}
	catch (CVMAbortException &err)
	{
		err.MaybePrintMessage();
		err.stacktrace.AppendFormat("Called from %s\n", call->PrintableName.GetChars());
		VMThrowException(std::current_exception());
		return 0;
	}
	catch (...)
	{
		VMThrowException(std::current_exception());
		return 0;
	}
}