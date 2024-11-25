# Макрос-обертка для функции strncpy
.macro strncpy (%dest, %src, %n)
    	# dest: буфер, в который производится запись
    	# src: исходная строка
    	# n: количество символов для копирования

    	la      a0 %dest
    	la      a1 %src
    	li      a2 %n
    	jal     STRNCPY
.end_macro
    
# Завершает программу
.macro exit
	li	a7 10
	ecall
.end_macro