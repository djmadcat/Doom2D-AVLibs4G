OPEN "NOTETAB.DAT" FOR OUTPUT AS 1

DIM A#(12)
DATA 220,0,247.5,264,0,297,0,330,352,0,396,0
FOR i = 1 TO 12: READ A#(i): NEXT i
FOR i = 1 TO 11
  IF A#(i) = 0 THEN A#(i) = SQR(A#(i - 1) * A#(i + 1))
NEXT i
A#(12) = SQR(A#(11) * A#(1) * 2)
FOR i = 1 TO 12: A#(i) = A#(i) / 220: NEXT i

N# = 1 / 4
C = 0: i = 1
DO
  W& = INT(N# * A#(i) * 1024)
  PRINT #1, W&; ",";
  C = C + 1
  i = i + 1: IF i > 12 THEN i = 1: N# = N# * 2
LOOP WHILE C < 12 * 8
CLOSE 1
END

