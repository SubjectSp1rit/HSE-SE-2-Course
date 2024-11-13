# макросы

# Читает целое число от пользователя и сохраняет в %reg
.macro read_int (%reg)
    	li a7 5
    	ecall
    	mv %reg a0
.end_macro

# Выводит в консоль число типа double
.macro print_double (%reg)
    	fmv.d fa0 %reg
    	li a7 3
    	ecall
.end_macro

# Вызывает подпрограмму COMPUTE_PI
.macro compute_pi
    	# Параметр N находится в a0
    	addi sp sp -8       	# Выделяем место на стеке для параметра
    	sw a0 0(sp)          	# Сохраняем параметр N из a0 в стек
    	jal ra COMPUTE_PI
    	addi sp sp 8        	# Восстанавливаем стек
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

# Завершает программу
.macro exit
	li a7 10
	ecall
.end_macro
