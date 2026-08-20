REM SPDX-License-Identifier: MIT
@echo off
cl /TP /std:c++latest /Iinclude /P /EP /C /DIRIS_ALLOY_GENERATE_PREPROCESSED /Fiinclude\iris\alloy\detail\preprocessed\temp.hpp include\iris\alloy\detail\tuple_impl.hpp
pushd include\iris\alloy\detail\preprocessed
type tuple_impl.hpp.pre.in temp.hpp tuple_impl.hpp.post.in > temp2.hpp
clang-format -i temp2.hpp
del /q tuple_impl.hpp
rename temp2.hpp tuple_impl.hpp
del temp.hpp
popd
