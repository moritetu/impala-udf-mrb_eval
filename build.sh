#!/bin/bash

set -eu

declare -A TARGETS

usage() {
  cat<<EOF
Usage: $(basename $0) [TARGETS]

TARGETS:
  all             Default target
  libmruby.a      Build mruby
  libmrb_eval.so  Build mrb_eval
  test            Run test for libmrb_eval.so
  clean           Remove temporary resources
EOF
}

#
# exec_if <target> && {
#   # do something
# }
#
exec_if() {
  local target="$1"

  if [ ${TARGETS[$target]:-0} -eq 1 ]; then
    echo "=> $target"
    return 0
  fi

  if [ ${TARGETS["all"]} -eq 1 ]; then
    # Not exec clean even if target is equal to all.
    if [ "$target" != "clean" ]; then
      echo "=> $target"
      return 0
    fi
  fi
  return 1
}


TARGETS["all"]=1
test $# -gt 0 && TARGETS["all"]=0

while [ -n "$*" ]
do
  case "$1" in
    "clean"|"libmruby.a"|"libmrb_eval.so"|"test"|"all")
      TARGETS["$1"]=1
      ;;
    --help|-h)
      usage && exit
      ;;
  esac
  shift
done

exec_if "clean" && {
  rm -rf vendor
}

exec_if "libmruby.a" && {
  (
    mkdir -p vendor
    if [ ! -d vendor/mruby ]; then
      git clone https://github.com/mruby/mruby.git vendor/mruby
      cd vendor/mruby && git reset --hard && git clean -fdx
      make CFLAGS="-fPIC"
    fi
  )
}

exec_if "libmrb_eval.so" && {
  TARGET=libmrb_eval.so
  cmake . && make

  if [ -f $TARGET ]; then
    cat<<EOF

@@@@@ Congratulations! @@@@@

Copy $TARGET to the arbitrary path on HDFS and create mrb_eval function in Impala;

$ hdfs dfs -copyFromLocal $TARGET /path/to/
$ impala-shell -i impala_host

> create function mrb_eval(string) returns string location '/path/to/$TARGET' symbol='MRBEval';
> select mrb_eval('(1..3).reduce(&:+)');
Query: select mrb_eval('(1..3).reduce(&:+)')
+----------------------------------------+
| default.mrb_eval('(1..3).reduce(&:+)') |
+----------------------------------------+
| 6                                      |
+----------------------------------------+
Fetched 1 row(s) in 0.02s

EOF
    fi
}

exec_if "test" && {
  ./mrb_eval-test
}
