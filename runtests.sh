#!/bin/bash
export GTEST_COLOR=yes
cd build
ctest --output-on-failure --verbose
exit $ctest_exit_code
