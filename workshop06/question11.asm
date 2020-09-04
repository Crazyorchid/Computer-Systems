// test header so that variables named in workshop
// questions will have known addresses we can test
	@x
	@y
	@sum
	@ptr
	@numbers
	@arr
// add your instructions after this line
push rbp

mov rbp, rsp

mov DWORD PTR [rbp-20], edi

mov DWORD PTR [rbp-4], 0

mov DWORD PTR [rbp-8], 0

.L3:

cmp DWORD PTR [rbp-4], 9   #While Condition Checking

jg .L4

mov eax, DWORD PTR [rbp-4]

cdqe

lea rdx, [0+rax*4]

mov rax, QWORD PTR [rbp-16]

add rax, rdx #add the address of number + value of x.

mov eax, DWORD PTR [rax] #Move the value at number[x] to reg eax.

add DWORD PTR [rbp-8], eax #Add the value at *number[x] to sum

add DWORD PTR [rbp-4], 1 #INcrement X .

jmp .L3 #Jump to next iteration.

.L4:

nop

pop rbp
