main:
	mv t0, zero # I-type (псевдо-инструкция, == addi t0, zero, 0)
	li t1, 1 # I-type (псевдо-инструкция, == addi t1, zero, 1)
	
	li a7, 5 # I-type (псевдо-инструкция, == addi a7, zero, 5)
	ecall # I-type (инструкция)
	mv t3, a0 # I-type (псевдо-инструкция, == addi t3, a0, 0)
fib:
	beqz t3, finish # B-type (псевдо-инструкция, == beq t3, zero, finish)
	add t2, t1, t0 # R-type (инструкция)
	mv t0, t1 # I-type (псевдо-инструкция, == addi t0, t1, 0)
	mv t1, t2 # I-type (псевдо-инструкция, == addi t1, t2, 0)
	addi t3, t3, -1 # I-type (инструкция)
	j	fib # J-type (псевдо-инструкция, == jal zero, fib)
finish:
	li a7, 1 # I-type (псевдо-инструкция, == addi a7, zero, 1)
	mv a0, t0 # I-type (псевдо-инструкция, == addi a0, t0, 0)
	ecall # I-type (инструкция)