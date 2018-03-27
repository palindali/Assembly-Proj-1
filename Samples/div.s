main:
addi	x10,x0,8
addi	x11,x0,2
jal	x1,mydiv
addi	x10,x0,0xA		# calls exit command (code 10)
ECALL				# end of program
mydiv:
add	x6,x0,x0		# i = 0
mydiv_test:
slt	x5,x10,x11		# if ( a < b )
bne	x5,x0,mydiv_end		# then get out of here
sub	x10,x10,x11		# else, a = a - b
addi	x6,x6,1			# and   i = i + 1
jal	x0,mydiv_test		# let's test again
mydiv_end:
add	x10,x0,x6		# result = i
add	x11,x0,x10		# rem   = a
jalr	x0,x1,0