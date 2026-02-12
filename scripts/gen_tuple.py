for i in range(1, 32 + 1):
    params = ", ".join(f"a{j}" for j in range(0, i))
    for j in range(0, i):
        print(f"#define IRIS_PP_TUPLE_ELEM_I_{i}_{j}({params}) a{j}")
    print()

for i in range(1, 32 + 1):
    print(
        f"#define IRIS_PP_TUPLE_TO_SEQ_I_{i}({', '.join(f'a{j}' for j in range(0, i))}) {''.join(f'(a{j})' for j in range(0, i))}"
    )
