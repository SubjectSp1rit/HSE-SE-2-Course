.include "macrolib.s"

.data
	msg_start: .asciz "If you want a deviation of less than 0.05%, enter a value greater than 10\n"
	msg_input_n: .asciz "n: "
	msg_out_of_range_exception: .asciz "n must be greater than 0\n"
.text
main:
    	# Чтение целого числа N от пользователя
	print_mstr msg_start
    	print_mstr msg_input_n
    	read_int a0          	# Читаем N в a0
    	
    	# Валидация введеного числа
    	li t0 1
    	blt a0 t0 out_of_range # Завершаем работу, если введенное значение меньше 1

    	# Вызов подпрограммы
    	compute_pi     # Вычисляем π с N итерациями
    	# Результат находится в fa0
    	
    	# Печать вычисленного значения π
    	print_double fa0     # Выводим вычисленное значение π
    	# Возвращаемого значения нет

    	# Завершение программы
    	exit
out_of_range:
	print_mstr msg_out_of_range_exception
	exit
