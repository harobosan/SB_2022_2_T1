S:      EQU     10
X:      EQU     2
FLAG:   EQU     0
Z:      EQU     -1
        SECTION TEXT
SCALE:  MACRO   &M, &N
        LOAD    &M
        MUL     &N
        STORE   &M
        ENDMACRO
SWAP:   MACRO   &M, &N
        LOAD    &M
        COPY    &N, &M
        STORE   &N
        ENDMACRO
SUPER:  MACRO
        SCALE   N, G
        SCALE   N+1, G
        IF      FLAG
        SWAP    N, N+1
        ENDMACRO
        INPUT   N
        COPY    N, A
        SCALE   A,
        COPY    A, B
        SCALE   B,
        SWAP    A, B
        COPY    B, N+1
        SUPER
        OUTPUT  N+1
        OUTPUT  N
        STOP
        SECTION DATA
N:      SPACE   X
A:      SPACE
B:      SPACE
G:      CONST   S
H:      CONST   FLAG
