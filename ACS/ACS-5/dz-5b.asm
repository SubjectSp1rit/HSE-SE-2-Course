.data
	msg_output: .asciz "Максимальное целое число, факториал которого помещается в 32 бита, равно "
	newline: .asciz "\n"
.text
main:	
	li a7 4
	la a0 msg_output
	ecall
	
        li a0 1
        li a1 1

        jal ra find_max_n_recursive
	# a0 теперь содержит максимальное n

        li a7 1
        ecall

        li a7 4
	la a0 newline
	ecall
        
        j exit
        
exit:
	li a7, 10
        ecall

find_max_n_recursive:
        # Сохраняем адрес возврата
        addi sp sp -4
        sw ra 0(sp)

        addi t0 a0 1		 # t0 = n + 1

        mul t1 a1 t0		 # t1 = f * (n + 1)

        # Проверка переполнения
        mulhu t2 a1 t0          # t2 = старшие 32 бита умножения
        bnez t2 overflow         # Если старшие биты не равны 0 - переполнение

        # Рекурсивный вызов, если переполнения нет
        mv a0 t0                 # n = n + 1
        mv a1 t1                 # f = f * (n + 1)
        jal ra find_max_n_recursive

        # После возврата из рекурсии, результат уже в a0
        j end_recursive

overflow:
        # n вызвало переполнение => n результат
        mv a0 a0	# a0 уже содержит n

end_recursive:
        # Восстановление стека
        lw ra 0(sp)
        addi sp sp 4
        jr ra






