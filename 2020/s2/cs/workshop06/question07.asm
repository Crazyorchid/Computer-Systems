// test header so that variables named in workshop
// questions will have known addresses we can test
	@x
	@y
	@sum
	@ptr
	@numbers
	@arr
// add your instructions after this line

@i

D = 0 

@sum

D = 0

(LOOP)

@i

D = M //D=i

@10

D = D - A // D = i -10

@END

D;JGT 

@i

D = M // D=i

@sum

M = D + M // sum = sum+1

@i

M = M + 1 // x = x+1

@LOOP

0 ; JMP 

(END)

@END

0 ; JMP // Infinite loop