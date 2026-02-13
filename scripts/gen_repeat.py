# SPDX-License-Identifier: MIT

for i in range(1, 8 + 1):
    print(f"#define IRIS_PP_REPEAT_{i}_I_0(macro, data)")
    print(f"#define IRIS_PP_REPEAT_{i}_I_1(macro, data) macro(0, data)")
    for j in range(1, 32):
        print(
            f"#define IRIS_PP_REPEAT_{i}_I_{j + 1}(macro, data) IRIS_PP_REPEAT_{i}_I_{j}(macro, data) macro({j}, data)"
        )
    print()
