# Examples
There are 4 different types of IR program shapes that a CFG solver needs to be able to handle. All shapes are provided in both C and iON IR.

> Each C program only uses `goto` statements rather than using conditionals. This is because iON IR only has `BEQ` so to demonstrate the C program equivelant of each IR program, we only use `goto` to simulate using conditionals.
## Straight-line DAG
```c
// Block A (entry)
int x = 10;
int y = 20;

// Block B
int z = x + y;
if (z == 30)
    goto block_C;

block_C:
    return z;
```

```plaintext
Block A (entry):
    %1 = MOV 10
    %2 = MOV 20
    JMP B

Block B:
    %3 = ADD %1, %2
    BEQ %3, 30, C

Block C (exit):
    RET %3
```


## Simple Loop
```c
int a = 10, b = 20;
block_A:
    a++;

int c = a + b;
if (c == 40)
    goto block_C;
else
    goto block_A;

block_C:
    return c;
```

```plaintext

```


## Nested Loop
```c
int a = 0, b = 0;

outer_block:
    if (a == 10) {
        inner_block:
            if (b == 5) {
                goto ret_block;
            } else {
                b++;
                goto inner_block;
            }
    } else {
        a++;
        goto outer_block;
    }

ret_block:
    return a + b;
```

```plaintext
```


## Diamond
```c
int a = 5;

if (a == 5) {
    a = 1;
    goto exit;
} else {
    a = -1;
    goto exit;
}

exit:
    return a;
```

```plaintext

```