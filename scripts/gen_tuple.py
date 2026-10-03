# SPDX-License-Identifier: MIT

for i in range(0, 32):
    params = ", ".join(f"a{j}" for j in range(0, i + 1))
    print(f"#define IRIS_ZZ_PP_TUPLE_ELEM_I_{i}({params}, ...) a{i}")
print()

for i in range(1, 32 + 1):
    print(
        f"#define IRIS_ZZ_PP_TUPLE_TO_SEQ_I_{i}({', '.join(f'a{j}' for j in range(0, i))}) {''.join(f'(a{j})' for j in range(0, i))}"
    )
