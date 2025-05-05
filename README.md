# Optimized hash table

Hash table optimized by using 3 methods:
1. Intrinsics.
2. Inline assembler
3. Assembler

## First version

Successfully found 95630230 words out of 10000000
finding lasted 56106.864700 ms

![v1 list](docs/v1_list.png)
![v1 graph](docs/v1_graph.svg)

## strcmp() optimization

### SSE optimizations:

Successfully found 95630230 words out of 10000000
finding lasted 46120.426100 ms

### all words support, check for length:

Successfully found 95630230 words out of 10000000
finding lasted 45449.313700 ms

Successfully found 95630230 words out of 10000000
finding lasted 43817.687000 ms

## different hash function (crc32)
Successfully found 95630230 words out of 10000000
finding lasted 27232.996300 ms

Successfully found 95630230 words out of 10000000
finding lasted 30144.921800 ms


### inligned

Successfully found 95630230 words out of 10000000
finding lasted 26482.548000 ms

Successfully found 95630230 words out of 10000000
finding lasted 26542.689400 ms

Successfully found 95630230 words out of 10000000
finding lasted 26542.172600 ms

Successfully found 95630230 words out of 10000000
finding lasted 26769.209800 ms

### avx
Successfully found 95630230 words out of 10000000
finding lasted 27277.673300 ms

Successfully found 95630230 words out of 10000000
finding lasted 27526.831500 ms

Successfully found 95630230 words out of 10000000
finding lasted 27651.796700 ms

### __m128i in structure for short names
Successfully found 95630230 words out of 10000000
finding lasted 24127.873800 ms

Successfully found 95630230 words out of 10000000
finding lasted 24247.744100 ms

Successfully found 95630230 words out of 10000000
finding lasted 24248.745600 ms

##### MurMur in these conditions
Successfully found 95630230 words out of 10000000
finding lasted 39763.906200 ms

### removed data dependency (cur_elem = cur_elem->next)
Successfully found 95630230 words out of 10000000
finding lasted 23883.408300 ms

Successfully found 95630230 words out of 10000000
finding lasted 23710.844200 ms

Successfully found 95630230 words out of 10000000
finding lasted 23728.583800 ms

Successfully found 95630230 words out of 10000000
finding lasted 23723.620700 ms

Successfully found 95630230 words out of 10000000
finding lasted 23780.985500 ms

### improved elements locality in bucket
Successfully found 95630230 words out of 10000000
finding lasted 11932.738000 ms

Successfully found 95630230 words out of 10000000
finding lasted 11965.033900 ms

Successfully found 95630230 words out of 10000000
finding lasted 11994.724800 ms

Successfully found 95630230 words out of 10000000
finding lasted 12016.170800 ms

Successfully found 95630230 words out of 10000000
finding lasted 11955.244300 ms

### cycle optimization (recoding)
Successfully found 95630230 words out of 10000000
finding lasted 10444.481000 ms

Successfully found 95630230 words out of 10000000
finding lasted 10381.750100 ms

Successfully found 95630230 words out of 10000000
finding lasted 10446.279000 ms

Successfully found 95630230 words out of 10000000
finding lasted 10434.678400 ms

Successfully found 95630230 words out of 10000000
finding lasted 10436.669900 ms

### array instead of list
Successfully found 95630230 words out of 10000000
finding lasted 5233.872100 ms

Successfully found 95630230 words out of 10000000
finding lasted 5291.568300 ms

Successfully found 95630230 words out of 10000000
finding lasted 5274.041800 ms

Successfully found 95630230 words out of 10000000
finding lasted 5241.296400 ms

Successfully found 95630230 words out of 10000000
finding lasted 5252.150100 ms

## ALIGNING TO 8
Successfully found 95630230 words out of 10000000
finding lasted 4928.520100 ms

Successfully found 95630230 words out of 10000000
finding lasted 4937.555400 ms

Successfully found 95630230 words out of 10000000
finding lasted 4950.207100 ms

Successfully found 95630230 words out of 10000000
finding lasted 4965.682500 ms

Successfully found 95630230 words out of 10000000
finding lasted 4983.560200 ms

### 1-byte crc32_optimized

```nasm
crc32_optimized:
        ; push rbp
        ; mov rbp, rsp

        xor eax, eax            ; hash = 0
        dec eax

        mov rcx, rsi

    .hash_loop:
        crc32 eax, BYTE [rdi]
        inc rdi

        loop .hash_loop

        xor eax, 0xFFFFFFFF

        ; pop rbp
ret
```

Successfully found 95630230 words out of 10000000
finding lasted 5049.042500 ms

Successfully found 95630230 words out of 10000000
finding lasted 5005.433800 ms

Successfully found 95630230 words out of 10000000
finding lasted 4962.294700 ms

Successfully found 95630230 words out of 10000000
finding lasted 5027.216300 ms

Successfully found 95630230 words out of 10000000
finding lasted 5034.388200 ms

### 8-byte crc32_optimized_8byte

```nasm
crc32_optimized_8byte:
        ; push rbp
        ; mov rbp, rsp

        add rsi, 7
        shr rsi, 3              ; divide by 8 rounding upward

        mov eax, 0xFFFFFFFF

        mov rcx, rsi

    .hash_loop:
        crc32 rax, QWORD [rdi]
        add rdi, 8

        loop .hash_loop

        xor eax, 0xFFFFFFFF

        ; pop rbp
ret
```

Successfully found 95630230 words out of 10000000
finding lasted 4442.338300 ms

Successfully found 95630230 words out of 10000000
finding lasted 4498.684600 ms

Successfully found 95630230 words out of 10000000
finding lasted 4433.636100 ms

Successfully found 95630230 words out of 10000000
finding lasted 4472.630300 ms

Successfully found 95630230 words out of 10000000
finding lasted 4459.023600 ms
