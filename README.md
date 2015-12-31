# impala-udf-mrb_eval

NOTE: This is an experimental implementation.

## Build

```sh
sh build.sh
```

## Usage

```
> create function mrb_eval(string) returns string location '/path/to/libmrb_eval.so' symbol='MRBEval' prepare_fn='MRBEvalPrepare' close_fn='MRBEvalClose';
> select mrb_eval('(1..3).reduce(&:+)');
Query: select mrb_eval('(1..3).reduce(&:+)')
+----------------------------------------+
| default.mrb_eval('(1..3).reduce(&:+)') |
+----------------------------------------+
| 6                                      |
+----------------------------------------+
Fetched 1 row(s) in 0.02s
```

