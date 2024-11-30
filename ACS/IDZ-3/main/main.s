.include "macrolib.s"

.global number_buffer
.global newline
.global space

.data
    	buffer: .space 10240            # Буфер для хранения текста (10 кбайт = 10 * 1024)
    	substring_buffer: .space 512    # Буфер для хранения подстроки
    	number_buffer: .space 12        # Буфер для хранения числа в виде строки
    	# Вывод для пользователя
    	input_file_msg: .asciz "Введите название входного файла: "
    	output_file_msg: .asciz "Введите название выходного файла: "
    	substring_msg: .asciz "Введите искомую подстроку: "
    	error_msg: .asciz "Ошибка при работе с файлом!\n"
    	output_data_to_console_msg: .asciz "Желаете вывести полученные данные в консоль?"
    	newline: .asciz "\n"
    	space: .asciz " "

.text
.globl main

main:
    	li a5, 10240        	# Максимальное количество символов для чтения

    	# Ввод названия входного файла
    	input_dialog (input_file_msg, buffer, a5)
    	mv a0, a1           	# Возврат адреса буфера в регистр a0
    	li a1, 100

    	# Удаляем символ новой строки из названия входного файла
    	la t0, buffer
    	remove_newline t0

    	# Открываем входной файл
    	la a0, buffer
    	li a1, 0            	# Открываем для чтения
    	li a7, 1024         	# Ваш системный вызов для открытия файла (предположим, что это пользовательский вызов)
    	ecall

    	# Проверяем успешность открытия входного файла
    	li t0, -1
    	beq a0, t0, error_exit

    	# Сохраняем дескриптор входного файла
    	mv s0, a0

    	# Ввод названия выходного файла
    	input_dialog (output_file_msg, buffer, a5)
    	mv a0, a1           	# Возврат адреса буфера в регистр a0
    	li a1, 100

    	# Удаляем символ новой строки из названия выходного файла
    	la t0, buffer
    	remove_newline t0

    	# Открываем выходной файл
    	la a0, buffer
    	li a1, 1            	# Открываем для записи
    	li a7, 1024
    	ecall

    	# Проверяем успешность открытия выходного файла
    	li t0, -1
    	beq a0, t0, error_exit

    	# Сохраняем дескриптор выходного файла
    	mv s1, a0

    	# Ввод подстроки для поиска
    	input_dialog (substring_msg, substring_buffer, a5)
    	mv a0, a1           	# Возврат адреса буфера в регистр a0
    	li a1, 100

    	# Удаляем символ новой строки из подстроки
    	la t0, substring_buffer
    	remove_newline t0

    	# Ввод Y/N для вывода данных в консоль
    	mv a1, a0
    	choose_dialog (output_data_to_console_msg)
    	mv a6, a0           	# Переносим выбор пользователя в регистр a6
    	mv a0, a1           	# Возврат адреса буфера в регистр a0

    	# Инициализируем счетчики
    	li s2, 0            	# Общий счетчик прочитанных байт
    	li s3, 0            	# Смещение в буфере
    	li t6, 10240        	# Максимальный размер файла (10 КБ)

read_loop:
    	# Вычисляем количество оставшихся байт для чтения
    	li t0, 10240
    	sub t0, t0, s2      	# t0 = 10240 - s2 (оставшиеся байты)

    	# Если t0 == 0, достигнут максимальный размер, завершаем чтение
    	beq t0, zero, end_read_loop

    	# Определяем, сколько байт читать в текущей итерации
    	li t1, 512
    	blt t0, t1, set_bytes_to_read
    	mv t2, t1         	# bytes_to_read = 512
    	j set_a1_a2
set_bytes_to_read:
    	mv t2, t0         	# bytes_to_read = оставшиеся байты
set_a1_a2:
    	# Устанавливаем адрес записи в буфер с учетом смещения
    	la t3, buffer
    	add a1, t3, s3      	# a1 = buffer + s3

    	# Устанавливаем количество байт для чтения
    	mv a2, t2         	# a2 = bytes_to_read

    	# Выполняем системный вызов чтения
    	mv a0, s0         	# Файловый дескриптор
    	li a7, 63
    	ecall

    	# Проверяем на ошибки чтения
    	blt a0, zero, error_exit

    	# Если a0 == 0, достигнут конец файла
    	beq a0, zero, end_read_loop

    	# Обновляем счетчики
    	add s2, s2, a0      	# s2 = s2 + a0 (общее количество прочитанных байт)
    	add s3, s3, a0      	# s3 = s3 + a0 (смещение в буфере)

    	# Переходим к следующей итерации
    	j read_loop

end_read_loop:
    	# Вызываем подпрограмму поиска подстроки
    	la a0, buffer           # Адрес текста
    	mv a1, s2             	# Длина текста
    	la a2, substring_buffer # Адрес подстроки
    	mv a3, s1             	# Дескриптор выходного файла
    	jal ra, find_substring

    	# Закрываем файлы
    	mv a0, s0
    	li a7, 57
    	ecall

    	mv a0, s1
    	li a7, 57
    	ecall

    	exit    		# Завершаем программу успешно

error_exit:
    	# Выводим сообщение об ошибке и завершаем программу
    	message_dialog (error_msg, 0)
    	exit    		# Завершаем программу