.data
	sep: 	.asciz "--------------------------\n"
	msg_input_num: .asciz "Введите число элементов массива (от 1 до 10): "
	msg_input_array_elem: .asciz "Введите число "
	msg_colon: .asciz ": "
	msg_sum_is: .asciz "Сумма введённых элементов равна: "
	msg_out_of_range_down: .asciz "Введённое число должно быть >= 1"
	msg_out_of_range_up: .asciz "Введённое число должно быть <= 10"
	msg_overflow: .asciz "Произошло переполнение.\n"
	msg_last_sum: .asciz "Сумма до переполнения равна: "
	msg_counter: .asciz "Чисел было просуммировано: "
	msg_num: .asciz "Число "
	msg_odd: .asciz " нечётное"
	msg_even: .asciz " чётное"
	newline: .asciz "\n"
.align 2
array: .space 64 # Выделяем 64 байта (больше, чем надо)
arrend: # Граница массива
.text
# Так как я выделил памяти больше необходимого, то цикл заполнения работает не от адреса array до
# адреса arrend, а при помощи счетчика от i = 1 (t2) до n (введеное пользователем число)
main:	
	# Вывод сообщения о необходимости ввода числа
	li a7 4
	la a0 msg_input_num
	ecall
	
	# Ввод значения
	li a7 5
	ecall
	mv t3 a0
	
	# Верхняя граница диапазона
	li s2 10
	
	li s3 0 # bool-тип для проверки положительного переполнения
	li s4 0 # bool-тип для проверки отрицательного переполнения
	li s6 1 # регистр-константа, хранящий 1 для вычитания
	li s7 2 # регистр, содержащий число-делитель 2
	
	# Проверка принадлежности вводимого числа диапазону [1, 10]
	blez t3 number_out_of_range_down
	bgt t3 s2 number_out_of_range_up
	
	li t2 1 # Счётчик (i)
	
	la t0 array # Указатель на текущий элемент массива
input_loop:
	# Вводим значения массива
	bgt t2 t3 end_loop # Как только счётчик дошел до n - завершаем цикл ввода данных
	
	# Выводим сообщение о вводе числа
	li a7 4
	la a0 msg_input_array_elem
	ecall
	
	mv a0 t2
	li a7 1
	ecall
	
	mv t2 a0 # Возврат числа i в регистр t2
	
	li a7 4
	la a0 msg_colon
	ecall
	
	# Ввод элемента массива
	li a7 5
	ecall
	
	sw a0 (t0) # Кладем только что введеное число в место, на которое указывает указатель массива
	addi t2 t2 1 # Увеличиваем счётчик i
	addi t0 t0 4 # Двигаемся по массиву дальше
	j input_loop
end_loop:
	la t0 array # После завершения заполнения массива возвращаем указатель на первый элемент массива
	li t2 1 # Возвращаем счётчик в значение 1
	li t4 0 # Сумма элементов равна 0 (до начала  суммирования)
	j output_loop
output_loop:
	bgt t2 t3 print_result # Бежим по массиву
	# Пробегаем по массиву и добавляем значение каждого элемента в регистр t4 (сумма эл-тов)
	lw a0 (t0) # Кладем в a0 текущий элемент массива
	
	mv t5 t4 # Предыдущее значение суммы (кладем t4 в t5)
	add t4 t4 a0 # Добавляем в сумму текущий элемент массива
	
	
	sgt s3 t5 t4
	slt s4 t5 t4
	
	bnez s3 check_num_bigger_than_zero # Если сумма уменьшилась после добавления элемента, делаем проверку что число было >0
	bnez s4 check_num_less_than_zero # Если сумма увеличилась после добавления. делаем проверку что число было <0
	
	j next_iter
next_iter:
	addi t2 t2 1 # Увеличиваем счётчик
	addi t0 t0 4 # Переходим к следующему элементу в памяти
	j output_loop
check_num_bigger_than_zero:
	bgtz a0 exception
	li s3 0
	j next_iter
check_num_less_than_zero:
	bltz a0 exception
	li s4 0
	j next_iter
exception:
	# Вывод сообщения о переполнении
	li a7 4
	la a0 msg_overflow
	ecall
	
	# Вывод последней суммы
	la a0 msg_last_sum
	ecall
	
	li a7 1
	mv a0 t5
	ecall
	
	li a7 4
	la a0 newline
	ecall
	
	# Вывод счётчика
	la a0 msg_counter
	ecall
	
	sub t2 t2 s6
	li a7 1
	mv a0 t2
	ecall 
	
	li a7 4
	la a0 newline
	ecall
	
	j exit
	
print_result:
	# Выводим сумму
	li a7 4
	la a0 msg_sum_is
	ecall
	
	li a7 1
	mv a0 t4
	ecall
	
	li a7 4
	la a0 newline
	ecall
	
	j exit
number_out_of_range_down:
	li a7 4
	la a0 msg_out_of_range_down
	ecall
	j exit
number_out_of_range_up:
	li a7 4
	la a0 msg_out_of_range_up
	ecall
	j exit
# Далее идет выполнение задания на 10 баллов (четные и нечетные числа)
exit:
	j end_main_program
end_main_program:
	la t0 array # После завершения заполнения массива возвращаем указатель на первый элемент массива
	li t2 1 # Возвращаем счётчик в значение 1
	j even_odd_checker
	
even_odd_checker:
	bgt t2 t3 final_exit # Бежим по массиву
	# Пробегаем по массиву и добавляем значение каждого элемента в регистр t4 (сумма эл-тов)
	lw a0 (t0) # Кладем в a0 текущий элемент массива
	
	rem s6 a0 s7
	bnez s6, print_odd
	j print_even
next_iter_even_odd:
	addi t2 t2 1 # Увеличиваем счётчик
	addi t0 t0 4 # Переходим к следующему элементу в памяти
	j even_odd_checker
print_odd:
	li a7 1
	ecall
	
	li a7 4 
	la a0 msg_odd
	ecall
	
	la a0 newline
	ecall
	j next_iter_even_odd
print_even:
	# Выводим число
	li a7 1
	ecall
	
	li a7 4 
	la a0 msg_even
	ecall
	
	la a0 newline
	ecall
	j next_iter_even_odd
final_exit:
	li a7 10
	ecall
	