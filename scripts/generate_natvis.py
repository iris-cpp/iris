#!/usr/bin/env python3
# Copyright 2026 The Iris Project Contributors
#
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

N = 32

print('<?xml version="1.0" encoding="utf-8"?>')
print('<AutoVisualizer')
print('    xmlns="http://schemas.microsoft.com/vstudio/debugger/natvis/2010"')
print('    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"')
print('    xsi:schemaLocation="http://schemas.microsoft.com/vstudio/debugger/natvis/2010 '
      'file:///C:/Program%20Files/Microsoft%20Visual%20Studio/18/Community/Xml/Schemas/1033/natvis.xsd"')
print('>')
print('    <Type Name="iris::alloy::tuple&lt;*&gt;">')

# DisplayString: 32 -> 1
for k in range(N, 0, -1):
    elems = ", ".join(f"{{_{i}}}" for i in range(k))
    print(f'        <DisplayString Optional="true">({elems})</DisplayString>')

print()
print('        <Expand>')

# Expand: 0 -> 31
for i in range(N):
    print(f'            <Item Name="[{i}]" Condition="_{i}, true" Optional="true">_{i}</Item>')

print('        </Expand>')
print('    </Type>')
print('</AutoVisualizer>')
