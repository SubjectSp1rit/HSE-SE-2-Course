.include "macrolib.s"

.global number_buffer
.global newline
.global space

.data
	buffer: .space 10240            # Буфер для хранения текста (10 КБ)
        substring_buffer: .space 512    # Буфер для хранения подстроки
        number_buffer: .space 12        # Буфер для хранения числа в виде строки

# Массивы названий файлов
input_filenames:
        .asciz "input1.txt\0"
        .asciz "input2.txt\0"
        .asciz "input3.txt\0"
        .asciz "input4.txt\0"
        .asciz "input5.txt\0"

output_filenames:
        .asciz "output1.txt\0"
        .asciz "output2.txt\0"
        .asciz "output3.txt\0"
        .asciz "output4.txt\0"
        .asciz "output5.txt\0"

substring_msg: .asciz "Введите искомую подстроку: "

test_success_msg: .asciz "Тест "
test_success_end: .asciz " выполнен успешно!\n"

error_msg: .asciz "Ошибка при работе с файлом!\n"
newline: .asciz "\n"
space: .asciz " "

num_tests: .word 5              # Количество тестов

.text
.globl main

main:
	# Запрашиваем у пользователя подстроку для поиска
        print_mstr substring_msg

        la a0 substring_buffer         # Адрес буфера для подстроки
        li a1 512                      # Максимальная длина подстроки
        li a7 8
        ecall

        # Удаляем символ новой строки из введенной подстроки
        la t5 substring_buffer
remove_newline:
        lb t6 0(t5)
        beq t6 zero end_remove_newline
        li a5 10
        beq t6 a5 set_zero          # Если символ = '\n' (код 10), заменить на '\0'
        addi t5 t5 1
        j remove_newline
set_zero:
        sb zero 0(t5)
end_remove_newline:

        # Инициализируем счетчик тестов
        li s5 0              # s5 = текущий тест (0..4)
        lw s6 num_tests      # s6 = общее количество тестов
test_loop:
        bge s5 s6 end_tests  # Если s5 >= s6, завершаем тесты

        # Получаем адрес имени входного файла
        la t2 input_filenames
        li t4 0
loop_input_filename:
        beq t4 s5 get_input_filename
        addi t4 t4 1
        addi t2 t2 12     # Смещение на 12 байт (размер строки с '\0')
        j loop_input_filename
get_input_filename:
        mv a0 t2            # a0 = адрес имени входного файла

        # Открываем входной файл
        li a1 0
        li a7 1024
        ecall
        li a4 -1
        beq a0 a4 error_exit
        mv s0 a0            # Файловый дескриптор входного файла

        # Счетчики для чтения файла
        li s2 0               # Общий счетчик прочитанных байт
        li s3 0               # Смещение в буфере
read_loop:
        # Количество оставшихся байт для чтения
        li t5 10240           # Максимальный размер файла (10 КБ)
        sub t5 t5 s2         # t5 = 10240 - s2
        beq t5 zero end_read_loop

        # Количество байт для чтения
        li t6 512
        blt t5 t6 set_bytes_to_read
        mv s4 t6            # bytes_to_read = 512
        j set_a1_a2
set_bytes_to_read:
        mv s4 t5            # bytes_to_read = оставшиеся байты
set_a1_a2:
        la a4 buffer
        add a1 a4 s3         # a1 = buffer + s3

        mv a2 s4

        # Читаем файл
        mv a0 s0
        li a7 63
        ecall

        # Проверяем на ошибки чтения
        blt a0 zero read_error

        # Если a0 == 0, достигнут конец файла
        beq a0 zero end_read_loop

        # Обновляем счетчики
        add s2 s2 a0         # s2 = s2 + a0
        add s3 s3 a0         # s3 = s3 + a0

        # Переходим к следующей итерации
        j read_loop
end_read_loop:
        # Закрываем входной файл
        mv a0 s0
        li a7 57
        ecall

        # Если размер файла превышает 10 КБ, завершаем программу
        li t5 10240
        bgt s2 t5 size_error

        # Получаем адрес имени выходного файла
        la t2 output_filenames
        li t4 0
loop_output_filename:
        beq t4 s5 get_output_filename
        addi t4 t4 1
        addi t2 t2 13    # Смещение на 13 байт (размер строки с '\0')
        j loop_output_filename
get_output_filename:
        mv a0 t2            # a0 = адрес имени выходного файла

        # Открываем выходной файл
        li a1 1
        li a7 1024
        ecall
        li a4 -1
        beq a0 a4 error_exit
        mv s1 a0            # Сохраняем дескриптор выходного файла

        # Сохраняем s5 и s6 на стеке перед вызовом find_substring
        addi sp sp -8
        sw s5 4(sp)
        sw s6 0(sp)

        # Вызываем подпрограмму поиска подстроки
        la a0 buffer           # Адрес текста
        mv a1 s2             # Длина текста
        la a2 substring_buffer # Адрес подстроки
        mv a3 s1             # Дескриптор выходного файла
        jal ra find_substring

        # Восстанавливаем s5 и s6 после вызова
        lw s6 0(sp)
        lw s5 4(sp)
        addi sp sp 8

        # Закрываем выходной файл
        mv a0 s1
        li a7 57
        ecall

        # Выводим сообщение о успешном выполнении теста
        print_mstr test_success_msg

        # Выводим номер теста
        addi t3 s5 1         # Номер теста (s5 от 0 до 4, поэтому прибавляем 1)
        print_int t3

        # Выводим оставшуюся часть сообщения
        print_mstr test_success_end

        # Увеличиваем счетчик тестов
        addi s5 s5 1
        j test_loop
read_error:
        # Обработка ошибки чтения
        print_mstr error_msg
        j error_exit
size_error:
        # Выводим сообщение о превышении размера файла
        print_mstr error_msg
        j error_exit
error_exit:
        exit
end_tests:
        exit