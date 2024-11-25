 .include "macrolib.s"

.data
	string1: 	.asciz	"Hello, world!"
	string2: 	.asciz "Bye, world!"
	buffer1: 	.space 30		# Буфер для теста 1
	buffer2: 	.space 30     		# Буфер для теста 2

	msg_enter: 	.asciz "Enter source string: "
	input_buffer:	.space 200	# Буфер для вводимой вручную строки

	newline:	.asciz "\n"

.text
.globl main

main:
	# Тест 1: Копирование жестко заданной строки с помощью макроса

    	strncpy buffer1, string1, 5

    	# Вывод результата
    	la      a0 buffer1
    	jal     print_string

    	# Вывод новой строки
    	la      a0 newline
    	jal     print_string

    	# Тест 2: Копирование другой строки с помощью макроса

    	strncpy buffer2, string2, 11

    	# Вывод результата
    	la      a0 buffer2
    	jal     print_string

    	# Вывод новой строки
    	la      a0 newline
    	jal     print_string

    	# Тест 3: Ввод строки с консоли и копирование при помощи strncpy

    	# Запрос ввода от пользователя
    	la      a0 msg_enter
    	jal     print_string

    	# Чтение строки от пользователя
    	la      a0 input_buffer		# Адрес буфера
    	li      a1 100			# Максимальная длина
    	li      a7 8
    	ecall

    	# Удаление символа новой строки
    	la      t0 input_buffer
remove_newline:
    	lb      t1 0(t0)
    	beqz    t1 end_remove_newline   # Если null-терминатор => конец
    	li      t2 10
    	beq     t1 t2 set_null         	# Если символ новой строки, заменить на null
    	addi    t0 t0 1
    	j       remove_newline
set_null:
    	sb      zero 0(t0)              # Замена на null
end_remove_newline:
    	# Копирование input_buffer в buffer1 с n = 15

    	la      a0 buffer1
    	la      a1 input_buffer
    	li      a2 15
    	jal     STRNCPY

    	# Вывод результата
    	la      a0 buffer1
    	jal     print_string

    	# Вывод новой строки
    	la      a0 newline
    	jal     print_string

    	# Завершение программы
    	exit

# Подпрограмма для вывода строки
# a0: адрес null-терминированной строки
print_string:
    	# Сохранение регистров на стеке
    	addi    sp sp -8
    	sw      ra 4(sp)
    	sw      a0 0(sp)

    	li      a7 4
    	ecall

    	# Восстановление стека
    	lw      ra 4(sp)
    	lw      a0 0(sp)
    	addi    sp sp 8
    	jr      ra