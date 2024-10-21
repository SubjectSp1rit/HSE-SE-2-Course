# Выводит в консоль переданную строку
.macro print_str (%str)
.data
	string: .asciz %str
.text
	li a7 4
	la a0 string
	ecall
.end_macro

# Выводит в консоль строку по адресу (mstr - memory string)
.macro print_mstr (%str)
	li a7 4
	la a0 %str
	ecall
.end_macro

# Выводит в консоль число
.macro print_int (%int)
	li a7 1
	mv a0 %int
	ecall
.end_macro

# Ввод числа в переданный регистр
.macro read_int
	jal ra READ_INT
.end_macro

# Заполнение массива (array ptr - указатель на начало массива, count - число элементов в массиве)
.macro fill_array (%array_ptr, %count)
	# Сохраняем данные на стеке
	addi sp sp -8
	sw %array_ptr 4(sp)
	sw %count 0(sp)
	jal ra FILL_ARRAY
	addi sp sp 8 # Освобождаем стек
.end_macro

# Вывод массива в консоль (array ptr - указатель на начало массива, count - число эл-тов в массиве)
.macro print_array (%array_ptr, %count)
	# Сохраняем данные на стеке
	addi sp sp -8
	sw %array_ptr 4(sp)
	sw %count 0(sp)
	jal ra PRINT_ARRAY
	addi sp sp 8 # Освобождаем стек
.end_macro

# Копирует данные из массива первого массива во второй с условием
# src_array_ptr - указатель на первый массив-источник
# dest_array_ptr - указатель на второй массив
# count_src_array - количество элементов в первом массиве
# condition - число-условие, по которому будут копироваться элементы
# После выполнения в а0 будет лежать число элементов в массиве В
.macro copy_array_with_condition (%src_array_ptr, %dest_array_ptr, %count_src_array, %condition)
	# Сохраняем данные на стеке
	addi sp sp -16	
	sw %src_array_ptr 12(sp)
	sw %dest_array_ptr 8(sp)
	sw %count_src_array 4(sp)
	sw %condition 0(sp)
	jal ra COPY_ARRAY_WITH_CONDITION
	addi sp sp 16 # Освобождаем стек
.end_macro

# Завершает программу
.macro exit
	li a7 10
	ecall
.end_macro
