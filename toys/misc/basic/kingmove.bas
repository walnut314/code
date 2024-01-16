OPTION GOSUB
1   DIM A$(10)
10 '       Supervisor
20  gosub 90    ' Initialization
30  gosub 150
40  gosub 190
rem 50  gosub 410
rem 60  gosub 290
rem 70  gosub 410
rem 80  gosub 60
90  '
100 CLS
rem 110 RANDOM
120 CU = 1
130 K$ = CHR$(174)' + CHR$(157) + Str(5,32) + CHR$(26) + STRING$(12,24) + STRING$(3,32) + CHR$(188) + STRING$(4,131) + CHR$(188) + CHR$(26) + STRING$(6,24) + CHR$(191) + STRING$(4,176) + CHR$(191)
131 PRINT K$
140 E$ = STRING$(4,8) + STRING$(10,32) + CHR$(26) + STRING$(10,8) + CHR$(26) + STRING$(10,32)
141 PRINT E$
145 RETURN
150 '       Set arrow to initial piece arrangement
160 A$(1) = K$: A$(2) = E$: A$(3) = E$: A$(4) = E$: A$(5) = E$
170 A$(6) = E$: A$(7) = E$: A$(8) = E$: A$(9) = E$
180 RETURN
190 '       Draw board
200 V$ = CHR$(195) + CHR$(191) + CHR$(208) + CHR$(191) + CHR$(209) + CHR$(191) + CHR$(208) + CHR$(191) + CHR$(200)
rem 210 PRINT: PRINT V$, V$, V$, V$, V$, V$, V$, V$, V$, V$, V$, V$, V$, V$;
211 LOCATE 3
220 PRINT STRING$(53,176)
221 LOCATE 324
230 PRINT STRING$(51,140)
231 LOCATE 644
240 PRINT STRING$(51,140)
241 LOCATE 963
250 PRINT STRING$(53,131)
260 POKE 15700, 191: POKE 15718, 191
270 POKE 16020, 191: POKE 16038, 191
280 RETURN

