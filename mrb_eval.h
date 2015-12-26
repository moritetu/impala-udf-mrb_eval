#ifndef MRB_EVAL_H
#define MRB_EVAL_H

#include <impala_udf/udf.h>

using namespace impala_udf;

StringVal MRBEval(FunctionContext* context, const StringVal& rb_code);

#endif
