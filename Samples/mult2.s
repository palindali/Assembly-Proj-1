main:
addi	x18,x0,20
addi	x9,x0,30
addi	x8,x0,0			# the product
while:
beq	x9,x0,endWhile
andi	x5,x9,0x01
beq	x5,x0,next
add	x8,x8,x18
next:
slli	x18,x18,1
srli	x9,x9,1
jal	x0,while
endWhile: