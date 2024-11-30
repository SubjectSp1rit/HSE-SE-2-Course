.global REMOVE_NEWLINE
.global find_substring
.global int_to_string
.global INPUT_DIALOG
.global MESSAGE_DIALOG
.global CHOOSE_DIALOG

# Service to display a message to a user and request a string input
# Параметры:
#    a0 - address of null-terminated string that is the message to user
#    a1 - address of input buffe
#    a2 - maximum number of characters to read (including the terminating null)
# Возвращаемое значение:
#    a1 - buffer contains the maximum allowable input string terminated with null.
INPUT_DIALOG:
	# Выделяем место на стеке для адреса возврата и сохраняем его
    	addi sp sp -4
    	sw ra 0(sp)

    	# Вызываем диалоговое окно
    	li a7 54
    	ecall

    	# Восстанавливаем стек
    	lw ra 0(sp)
    	addi sp sp 4
    	jr ra
    	
# Service to display a message to user
# Параметры:
#    a0 - address of null-terminated string that is the message to user
#    a1 - the type of the message to the user
# Возвращаемое значение:
#    Нет
MESSAGE_DIALOG:
	# Выделяем место на стеке для адреса возврата и сохраняем его
    	addi sp sp -4
    	sw ra 0(sp)

    	# Вызываем диалоговое окно
    	li a7 55
    	ecall

    	# Восстанавливаем стек
    	lw ra 0(sp)
    	addi sp sp 4
    	jr ra
    	
# Service to display a message to user
# Параметры:
#    a0 - address of null-terminated string that is the message to user
# Возвращаемое значение:
#    a0 = Yes (0), No (1), or Cancel(2)
CHOOSE_DIALOG:
	# Выделяем место на стеке для адреса возврата и сохраняем его
    	addi sp sp -4
    	sw ra 0(sp)

    	# Вызываем диалоговое окно
    	li a7 50
    	ecall

    	# Восстанавливаем стек
    	lw ra 0(sp)
    	addi sp sp 4
    	jr ra

# Подпрограмма удаления символа новой строки
# Параметры:
#    4(sp) - адрес строки
# Возвращаемое значение: 
#    Нет
REMOVE_NEWLINE:
	addi sp sp -4
	sw ra 0(sp)
	
	# Восстанавливаем значение адреса строки из стека
	lw t1 4(sp)
remove_newline_loop:
    	lb t2 (t1)
    	beqz t2 remove_newline_end
    	li t3 10  	# Код символа новой строки
    	beq t2 t3 remove_newline_found
    	addi t1 t1 1
    	j remove_newline_loop
remove_newline_found:
    	sb zero (t1)
remove_newline_end:
    	lw ra 0(sp)	# Восстанавливаем адрес возврата из стека
    	addi sp sp 4 	# Восстанавливаем стек
    	jr ra 		# Выходим из подпрограммы

# Подпрограмма поиска подстроки
# Использует s3-s8 для хранения промежуточных значений
# Параметры:
#    4(sp) - адрес текста
#    8(sp) - длина текста
#    12(sp) - адрес подстроки
#    16(sp) - дескриптор выходного файла
# Возвращаемое значение:
#    Нет
find_substring:
    	addi sp sp -4
    	sw ra 0(sp)
    
    	mv s3 a0   	# Адрес текста
    	mv s4 a1   	# Длина текста
    	mv s5 a2   	# Адрес подстроки
    	mv s8 a3   	# Дескриптор выходного файла
    	mv s9 a6 	# Выбор пользователя Y/N
    	li s6 0    	# Текущий индекс в тексте
    
    	# Находим длину подстроки
    	mv t0 a2
    	li s7 0    # Длина подстроки
strlen_loop:
    	lb t1 (t0)
    	beqz t1 strlen_done
    	addi s7 s7 1
    	addi t0 t0 1
    	j strlen_loop
strlen_done:
# Основной цикл поиска
main_search_loop:
    	bge s6 s4 search_done    # Если достигли конца текста
    
    	# Проверяем, есть ли достаточно символов для подстроки
    	sub t0 s4 s6
    	blt t0 s7 search_done
    
    	# Сравниваем подстроку с текущей позицией
    	li t0 0    # Индекс в подстроке
    	mv t1 s6   # Текущий индекс в тексте
compare_loop:
    	bge t0 s7 match_found
    	add t2 s3 t1
    	lb t3 (t2)     # Символ из текста
    	add t2 s5 t0
    	lb t4 (t2)     # Символ из подстроки
    	bne t3 t4 no_match
    	addi t0 t0 1
    	addi t1 t1 1
    	j compare_loop

match_found:
    	# Конвертируем индекс в строку
    	mv a0 s6
    	la a1 number_buffer
    	jal ra int_to_string
    
    	# Записываем число в файл
    	mv a0 s8       # Дескриптор выходного файла
    	la a1 number_buffer
    	mv a2 a3       # Длина строки (возвращается в a3)
    	li a7 64
    	ecall
    
    	mv a0 a1
    	# Если пользователь выбрал YES - выводим число в консоль
    	beqz a6 write_symbol_to_console
	return1: # Метка для возврата
    
    	# Записываем пробел в файл
    	mv a0 s8
    	la a1 space
    	li a2 1
    	li a7 64
    	ecall
    
    	mv a0 a1
    	# Если пользователь выбрал YES - выводим пробел в консоль
    	beqz a6 write_space_to_console
    	return2: # Метка для возврата
no_match:
    	addi s6 s6 1
    	j main_search_loop
search_done:
    	# Записываем символ новой строки
    	mv a0 s8
    	la a1 newline
    	li a2 1
    	li a7 64
    	ecall
    
    	# Восстанавливаем адрес возврата
    	lw ra 0(sp)
    	addi sp sp 4
    	jr ra
write_symbol_to_console:
	li a7 4
	ecall
	j return1
write_space_to_console:
	li a7 4
	ecall
	j return2

# Подпрограмма конвертации целого числа в строку
# Параметры:
#    a0 - число для конвертации
#    a1 - адрес буфера для результата
# Возвращаемое значение:
#    a3 - длину полученной строки
int_to_string:
    	mv t0 a0       # Сохраняем число
    	mv t1 a1       # Адрес буфера
    	li t2 0        # Счетчик цифр
    
    	# Обрабатываем случай с нулем
    	bnez t0 not_zero
    	li t3 48       # ASCII код '0'
    	sb t3 (t1)
    	li t3 0
    	sb t3 1(t1)
    	li a3 1
    	ret
not_zero:
    	# Конвертируем число в строку (в обратном порядке)
convert_loop:
    	beqz t0 reverse_string
    	# Получаем последнюю цифру
    	li t3 10
    	rem t4 t0 t3      # Остаток от деления на 10
    	div t0 t0 t3      # Делим число на 10
    	# Конвертируем в ASCII и сохраняем
    	addi t4 t4 48     # Добавляем ASCII код '0'
    	add t5 t1 t2
    	sb t4 (t5)
    	addi t2 t2 1
    	j convert_loop

reverse_string:
    	mv a3 t2           # Сохраняем длину строки
    	# Добавляем нулевой байт в конец
    	add t3 t1 t2
    	sb zero (t3)
    	# Переворачиваем строку
    	mv t3 t1           # Начало строки
    	add t4 t1 t2
    	addi t4 t4 -1      # Конец строки
reverse_loop:
    	bge t3 t4 reverse_done
    	lb t5 (t3)
    	lb t6 (t4)
    	sb t6 (t3)
    	sb t5 (t4)
    	addi t3 t3 1
    	addi t4 t4 -1
    	j reverse_loop
reverse_done:
    	ret