TITLE   Dot Matrix Display - DANIAL HOSSAIN DANI

        #MAKE_EXE#

DSEG SEGMENT 'DATA'


Group1  DB 7Fh,41h,41h,41h,3Eh   ; D
        DB 7Eh,11h,11h,11h,7Eh   ; A
        DB 7Fh,04h,08h,10h,7Fh   ; N
        DB 41h,41h,7Fh,41h,41h   ; I
        DB 7Eh,11h,11h,11h,7Eh   ; A
        DB 7Fh,40h,40h,40h,40h   ; L
        DB 00h,00h,00h,00h,00h
        DB 00h,00h,00h,00h,00h

Group2  DB 7Fh,08h,08h,08h,7Fh   ; H
        DB 3Eh,41h,41h,41h,3Eh   ; O
        DB 26h,49h,49h,49h,32h   ; S
        DB 26h,49h,49h,49h,32h   ; S
        DB 7Eh,11h,11h,11h,7Eh   ; A
        DB 41h,41h,7Fh,41h,41h   ; I
        DB 7Fh,04h,08h,10h,7Fh   ; N
        DB 00h,00h,00h,00h,00h

Group3  DB 7Fh,41h,41h,41h,3Eh   ; D
        DB 7Eh,11h,11h,11h,7Eh   ; A
        DB 7Fh,04h,08h,10h,7Fh   ; N
        DB 41h,41h,7Fh,41h,41h   ; I
        DB 00h,00h,00h,00h,00h
        DB 00h,00h,00h,00h,00h
        DB 00h,00h,00h,00h,00h
        DB 00h,00h,00h,00h,00h

ClearBuf DB 40 DUP(0)

DSEG ENDS

SSEG SEGMENT STACK 'STACK'
        DW 100h DUP(?)
SSEG ENDS

CSEG SEGMENT 'CODE'

START PROC FAR

    MOV AX, DSEG
    MOV DS, AX
    MOV ES, AX

INFINITE:

    MOV DX, 2000h
    MOV BX, OFFSET Group1
    CALL DISPLAY_40

    MOV DX, 2000h
    MOV BX, OFFSET ClearBuf
    CALL DISPLAY_40

    MOV DX, 2000h
    MOV BX, OFFSET Group2
    CALL DISPLAY_40

    MOV DX, 2000h
    MOV BX, OFFSET ClearBuf
    CALL DISPLAY_40

    MOV DX, 2000h
    MOV BX, OFFSET Group3
    CALL DISPLAY_40_FIXED       

    MOV DX, 2000h
    MOV BX, OFFSET ClearBuf
    CALL DISPLAY_40

    JMP INFINITE

START ENDP




DISPLAY_40 PROC NEAR
    MOV SI, 0
    MOV CX, 40
SEND_LOOP:
    MOV AL,[BX+SI]
    OUT DX, AL
    INC SI
    INC DX
    LOOP SEND_LOOP
    RET
DISPLAY_40 ENDP




DISPLAY_40_FIXED PROC NEAR
    MOV SI, 0
    MOV CX, 20     
SEND_LOOP2:
    MOV AL,[BX+SI]
    OUT DX, AL
    INC SI
    INC DX
    LOOP SEND_LOOP2
    RET
DISPLAY_40_FIXED ENDP

CSEG ENDS

END START