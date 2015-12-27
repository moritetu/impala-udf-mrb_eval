#include <string>
#include "mrb_eval.h"
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/string.h"

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

  mrb_state *mrb = mrb_open();
  if (!mrb) {
    return StringVal::null();
  }
  mrb_value value = mrb_load_nstring(mrb, reinterpret_cast<char*>(rb_code.ptr), rb_code.len);
  if (mrb->exc) {
    mrb_close(mrb);
    return StringVal::null();
  }
  const char *retstr = mrb_string_value_ptr(mrb, value);
  StringVal result = StringVal::CopyFrom(context,
					 reinterpret_cast<const uint8_t*>(retstr),
					 strlen(retstr));
  mrb_close(mrb);
  return result;
}
