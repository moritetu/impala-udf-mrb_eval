#include <string>
#include "mrb_eval.h"
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/string.h"


#if true
#define ARENA_SAVE \
  int ai = mrb_gc_arena_save(mrb);
#define ARENA_RESTORE \
  mrb_gc_arena_restore(mrb, ai);
#else
#define ARENA_SAVE
#define ARENA_RESTORE
#endif


void MRBEvalPrepare(FunctionContext* context, FunctionContext::FunctionStateScope scope)
{
  if (scope != FunctionContext::THREAD_LOCAL) return;
  if (!context->IsArgConstant(0)) return;
  assert(context->GetArgType(0)->type == FunctionContext::TYPE_STRING);

  StringVal *code = reinterpret_cast<StringVal*>(context->GetConstantArg(0));
  if (code->is_null) return;

  mrb_state *mrb = mrb_open();
  if (!mrb) {
    context->SetError("Failed to initialize mrb");
    return;
  }
  context->SetFunctionState(scope, mrb);
}

void MRBEvalClose(FunctionContext* context, FunctionContext::FunctionStateScope scope)
{
  if (scope != FunctionContext::THREAD_LOCAL) return;
  mrb_state *mrb = reinterpret_cast<mrb_state*>(context->GetFunctionState(scope));
  if (mrb) {
    mrb_close(mrb);
  }
  context->SetFunctionState(scope, NULL);
}

static StringVal MRBEvalRun(FunctionContext *context,
				mrb_state *mrb,
				const StringVal& rb_code)
{
  ARENA_SAVE;
  mrb_value value = mrb_load_nstring(mrb, reinterpret_cast<char*>(rb_code.ptr), rb_code.len);
  StringVal retval;
  // Ignore exceptions
  if (mrb->exc) {
    retval = StringVal::null();
    mrb->exc = 0;
  } else {
    const char *retstr = mrb_string_value_ptr(mrb, value);
    retval = StringVal::CopyFrom(context,
				 reinterpret_cast<const uint8_t*>(retstr),
				 strlen(retstr));
  }
  ARENA_RESTORE;
  return retval;
}

//
// mrb_eval evaluates mruby code.
//
// select mrb_eval('(1..3).reduce(&:+)'); => 6
//
//
StringVal MRBEval(FunctionContext* context, const StringVal& rb_code)
{
  if (rb_code.is_null) {
    return StringVal::null();
  }

  mrb_state *mrb;
  void *state = context->GetFunctionState(FunctionContext::THREAD_LOCAL);
  if (state != NULL) {
    mrb = reinterpret_cast<mrb_state*>(state);
    return MRBEvalRun(context, mrb, rb_code);
  }

  mrb = mrb_open();
  if (!mrb) {
    context->AddWarning("Failed to initialize mrb");
    return StringVal::null();
  }

  StringVal result = MRBEvalRun(context, mrb, rb_code);
  mrb_close(mrb);
  return result;
}
