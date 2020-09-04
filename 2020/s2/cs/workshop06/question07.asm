// test header so that variables named in workshop
// questions will have known addresses we can test
	@x
	@y
	@sum
	@ptr
	@numbers
	@arr
// add your instructions after this line
@x
M=0
@sum
M=0
(LOOP)
@x
D=M
@9
D=D-A
@END
D;JGT
@x
D=M
@sum
M=D+M
@x
M=M+1
@LOOP
0;JMP
(END)
@END
0;JMP
