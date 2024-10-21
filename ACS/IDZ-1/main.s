.data
	msg_input_n:    .asciz "Input array size (1 <= n <= 10): "
	msg_out_of_range: .asciz "Value is outside of range 1 <= n <= 10"
	newline: .asciz "\n"
	msg_input_x:    .asciz "Input number x: "
	arrayA: .space 40 # Выделяем память
	arrendA: # Граница массива

	arrayB: .space 40
	arrendB:

.include "macrolib.s"
.align 2
.text
.global main

main:
	print_mstr msg_input_n
	read_int
	
	mv s0 a0 # s0 - число эл-тов
	
	# Валидация данных (1 <= n <= 10)
	li t1 10
	bgt s0 t1 out_of_range
	li t1 1
	blt s0 t1 out_of_range
	
	la s1 arrayA # s1 - указатель на начало массива А
	
	# Передаем в функцию s1 - указатель на начало массива А и s0 - число элементов в массиве А.
	fill_array s1 s0 # Заполняем массив А
	# Возвращаемого результата нет
	
	la s2 arrayB # s2 - указатель на начало массива В
	
	print_mstr msg_input_x
	read_int
	# сейчас в а0 лежит число Х
	
	# Передаем в функцию s1 - указатель на начало массива А, s2 - указатель на начало массива В, s0 - число элементов в массиве А, a0 - число Х
	copy_array_with_condition s1 s2 s0 a0 # Заполняем массив В
	# Функция вернула в а0 длину массива В
	mv s4 a0 # После выполнения подпрограммы в а0 хранится число элементов в массиве В
	
	# Передаем в функцию s2 - указатель на массив В, s4 - число элементов в массиве В
	print_array s2 s4
	# Возвращаемого результата нет
	
	exit
out_of_range:
	print_mstr msg_out_of_range
	exit