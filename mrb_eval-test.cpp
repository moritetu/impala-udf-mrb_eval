#include <impala_udf/udf-test-harness.h>
#include "mrb_eval.h"

using namespace impala;
using namespace impala_udf;
using namespace std;

int main(int argc, char* argv[])
{
  bool passed = true;

  passed &= UdfTestHarness::ValidateUdf<StringVal, StringVal>(
    MRBEval, StringVal::null(), StringVal::null());
  passed &= UdfTestHarness::ValidateUdf<StringVal, StringVal>(
    MRBEval, StringVal("invalid ruby code"), StringVal::null());
  passed &= UdfTestHarness::ValidateUdf<StringVal, StringVal>(
    MRBEval, StringVal("raise \"raise error returns null\""), StringVal::null());
  passed &= UdfTestHarness::ValidateUdf<StringVal, StringVal>(
    MRBEval, StringVal("(1..3).reduce(&:+)"), StringVal("6"));
  passed &= UdfTestHarness::ValidateUdf<StringVal, StringVal>(
    MRBEval, StringVal("class Hello; def self.say(name); \"hello #{name}\"; end; end; Hello.say \"mrb_eval\""), StringVal("hello mrb_eval"));

  cout << "Tests " << (passed ? "Passed." : "Failed.") << endl;
  return !passed;
}
