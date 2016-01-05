# impala-udf-mrb_eval

NOTE: This is an experimental implementation.
For huge data, this cannot make a good performance.

## Build

You have to install the following libraries.

```
sudo yum install gcc-c++ cmake boost-devel
sudo yum install impala-udf-devel
```

Then execute the following command.

```sh
sh build.sh
```

If you can build successfully, you will see `libmrb_eval.so` in the project top.

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