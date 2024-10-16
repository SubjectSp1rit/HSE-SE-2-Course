.data
	msg_output: .asciz "Максимальное целое число, факториал которого помещается в 32 бита, равно "
	newline: .asciz "\n"
.text
main:
	li a7 4
	la a0 msg_output
	ecall
	
        # Переходим в подпрограмму
        jal ra find_max_n
        # a0 теперь содержит максимальное n
        
        li a7 1
        ecall
        
        li a7 4
        la a0 newline
        ecall
        
        # Завершение программы
        j exit

exit:
	li a7 10
	ecall

# Подпрограмма
find_max_n:
        # Сохраняем адрес возврата
        addi sp sp -4
        sw ra 0(sp)

        li t0 0		 # t0 = n
        li t1 1		 # t1 = накопленный результат

loop_start:
        addi t0 t0 1 		# n += 1

        # Вычисляем старшие биты
        mulhu t2 t1 t0 	 # t2 = результат * счетчик

        # Вычисляем младшие биты
        mul t3 t1 t0    # t3 = результат * счетчик

        # Проверка переполнения (старшие биты не должен содержать ничего, кроме 0)
        bnez t2 overflow

        mv t1 t3

        j loop_start

overflow:
        # n вызвало переполнение => n - 1 является максимальным
        addi t0 t0 -1
        mv a0 t0

        # Восстанавливаем адрес возврата из стека
        lw ra 0(sp)
        addi sp sp 4
        jr ra
 
 
 
 
 
 
 
 
 