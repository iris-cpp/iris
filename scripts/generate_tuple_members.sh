#!/usr/bin/sh
# SPDX-License-Identifier: MIT
g++ -Iinclude -E -P -DIRIS_ALLOY_GENERATE_PREPROCESSED include/iris/alloy/detail/tuple_impl.hpp > include/iris/alloy/detail/preprocessed/temp.hpp
cd include/iris/alloy/detail/preprocessed
cat tuple_impl.hpp.pre.in temp.hpp tuple_impl.hpp.post.in > temp2.hpp
clang-format -i temp2.hpp
mv temp2.hpp tuple_impl.hpp
rm temp.hpp
