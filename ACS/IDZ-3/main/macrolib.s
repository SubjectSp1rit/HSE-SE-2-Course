# Праметры:
#    address - адрес строки
.macro remove_newline (%address)
	# Сохраняем адрес строки на стеке
	addi sp sp -4
	sw %address 0(sp)
	
	# Переходим в подпрограмму
	jal ra REMOVE_NEWLINE
	
	# Восстанавливаем стек
	addi sp sp 4
.end_macro

# Вызов диалогового окна для сбора информации
# Параметры:
#    message - address of null-terminated string that is the message to user
#    buffer - address of input buffer
#    symbols_count - maximum number of characters to read (including the terminating null)
.macro input_dialog (%message, %buffer, %symbols_count)
    	la a0 %message          # Передаем адрес сообщения в a0
    	la a1 %buffer         	# Передаем адрес буфера в a1
    	mv a2 %symbols_count
    	jal ra INPUT_DIALOG
.end_macro

# Вызов диалогового окна для вывода сообщения
# Параметры:
#    message - address of null-terminated string that is the message to user
#    type - the type of the message to the user
.macro message_dialog (%message, %type)
    	la a0 %message          # Передаем адрес сообщения в a0
    	li a1 %type         	# Передаем тип окна в a1
    	jal ra MESSAGE_DIALOG
.end_macro

# Вызов диалогового окна для выбора Yes/No
# Параметры:
#    message - address of null-terminated string that is the message to user
.macro choose_dialog (%message)
    	la a0 %message          # Передаем адрес сообщения в a0
    	jal ra CHOOSE_DIALOG
.end_macro

# Завершает программу
.macro exit
	li a7 10
	ecall
.end_macro