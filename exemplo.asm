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
        COPY    &N,&M
        STORE   &N
        ENDMACRO
SUPER:  MACRO;isto e um comentario
        SCALE   N, H
        SCALE   N+1, H
        IF      FLAG
        SWAP    N, N+1
        ENDMACRO
LABEL:                  ;acabaram as macros
        INPUT   N
                   
        COPY    N,A
        SCALE   A,G
        COPY    A,B
        SCALE   B,H
        SWAP    N A
        coPY    B,N+1
        swap    N N+1
        sCale   A,h


        sUpER
        OUTPUT  A;b c d e
        OUTPUT , N
        OUTPUT  N+1,,,,
        STOP

        SECTION DATA
N:      SPACE   X
A:      SPACE
B:      SPACE
G:      CONST   S
H:      CONST   Z