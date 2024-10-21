.data
	newline: .asciz "\n"
	msg_output: .asciz "B: "
	space: .asciz " "
.text
.globl READ_INT
.globl FILL_ARRAY
.globl PRINT_ARRAY
.globl COPY_ARRAY_WITH_CONDITION

# Подпрограмма для считывания целого числа
READ_INT:
    # Сохраняем адрес возврата на стеке
    addi sp sp -4
    sw ra 0(sp)

    # Считываем число (автоматически лежит в регистре а0)
    li a7 5
    ecall

    lw ra 0(sp)           # Восстанавливаем адрес возврата
    addi sp sp 4         # Освобождаем стек
    jr ra                  # Выходим из подпрограммы

# Подпрограмма для заполнения массива
FILL_ARRAY:
	addi sp sp -4
	sw ra 0(sp) # Сохраняем адрес возврата
	
	lw t2 8(sp) # Указатель на массив А
	lw t1 4(sp) # Число эл-тов в массиве А
	
	li t0 1 # Инициализируем счетчик
fill_cycle:
	bgt t0 t1 end_fill_cycle
	
	jal ra READ_INT # Считываем число
	sw a0 (t2) # Кладем число в массив
	
	addi t0 t0 1 # Увеличиваем счетчик
	addi t2 t2 4 # Сдвигаем указатель массива
	j fill_cycle
end_fill_cycle:
	lw ra 0(sp) # Восстанавливаем адрес возврата из стека
	addi sp sp 4 # Освобождаем стек
    	jr ra # Выходим из подпрограммы

# Подпрограмма для вывода массива в консоль
PRINT_ARRAY:
	addi sp sp -4
	sw ra 0(sp)
	lw t2 8(sp) # Указатель на начало массива
	lw t1 4(sp) # Число элементов в массиве
	
	# Выводим строку
	li a7 4
	la a0 msg_output
	ecall
	
	li t0 1 # Инициализируем счетчик
print_cycle:
	bgt t0 t1 end_print_cycle
	
	lw a0 (t2) # Кладем число в регистр a0
	
	# Выводим число
	li a7 1
	ecall
	
	# Ставим пробел
	li a7 4
	la a0 space
	ecall
	
	addi t0 t0 1 # Увеличиваем счетчик
	addi t2 t2 4 # Сдвигаем указатель массива
	j print_cycle
end_print_cycle:
	lw ra 0(sp) # Восстанавливаем адрес возврата из стека
	addi sp sp 4 # Освобождаем стек
    	jr ra # Выходим из подпрограммы

# Подпрограмма для копирования массива
COPY_ARRAY_WITH_CONDITION:
	addi sp sp -4
	sw ra 0(sp) # Сохраняем адрес возврата
	lw t2 16(sp) # Указатель на первый массив
	lw t4 12(sp) # Указатель на второй массив
	lw t1 8(sp) # Количество элементов
	lw t3 4(sp) # Число-условие
	
	li t0 1 # Инициализируем счетчик
	li t5 0 # Инициализируем счетчик элементов в массиве В
copy_cycle:
	bgt t0 t1 end_copy_cycle
	
	lw a0 (t2) # Кладем число в регистр a0
	bne a0 t3 append_elem_to_array
	
	addi t0 t0 1 # Увеличиваем счетчик
	addi t2 t2 4 # Сдвигаем указатель массива
	j copy_cycle
append_elem_to_array:
	addi t5 t5 1 # Увеличиваем кол-во эл-тов массива В на 1
	sw a0 (t4) # Кладем число в массив В
	
	addi t4 t4 4 # Сдвигаем указатель массива В
	
	addi t0 t0 1 # Увеличиваем счетчик
	addi t2 t2 4 # Сдвигаем указатель массива
	j copy_cycle
end_copy_cycle:
	mv a0 t5 # Кладем в регистр а0 количество эл-тов массива В
	lw ra 0(sp) # Восстанавливаем адрес возврата из стека
	addi sp sp 4 # Освобождаем стек
    	jr ra # Выходим из подпрограммы
