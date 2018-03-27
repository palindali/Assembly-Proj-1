addi	x18,x0,20
addi	x9,x0,30
addi	x8,x0,0			# the product
while:
bge	x0,x18,endWhile
add	x8,x8,x9
addi	x18,x18,-1
jal	x0,while
endWhile: