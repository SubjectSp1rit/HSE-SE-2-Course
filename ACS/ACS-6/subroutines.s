.text
.globl STRNCPY

#   a0: dest (адрес буфера, в который производится запись)
#   a1: src (адрес исходной строки)
#   a2: n (количество символов для копирования)
# Возвращаемое значение:
#   a0: dest (адрес буфера, в который произвелась запись)
STRNCPY:
    addi    sp sp -16       	# Выделяем места на стеке
    sw      ra 12(sp)       	# Сохранение адреса возврата на стеке
    # Сохранение регистров s0-s2
    sw      s0 8(sp)
    sw      s1 4(sp)
    sw      s2 0(sp)

    # Инициализация указателей и счетчиков
    mv      s0 a0            	# s0 = dest
    mv      s1 a1            	# s1 = src
    mv      s2 a2            	# s2 = n

strncpy_loop:
    beqz    s2 strncpy_end   	# Если n == 0 => завершаем цикл
    lb      t0 0(s1)         	# Загрузка байт из src
    addi    s2 s2 -1         	# Декрементирование n

    beqz    t0 strncpy_null_src # Если достигнут null-терминатор => заполняем остаток null

    sb      t0 0(s0)         	# Записываем байт в dest
    addi    s0 s0 1         	# Инкрементируем указатели
    addi    s1 s1 1
    j       strncpy_loop

strncpy_null_src:
    # Заполнение dest нулевыми байтами
    sb      zero 0(s0)       	# Записываем null-байт
    addi    s0 s0 1         	# Инкрементируем указатель
    j       strncpy_loop

strncpy_end:
    # Возвращаемое значение уже в a0 (dest)
    lw      ra 12(sp)        	# Восстанавливаем регистры
    lw      s0 8(sp)
    lw      s1 4(sp)
    lw      s2 0(sp)
    addi    sp sp 16        	# Восстанавливаем стек
    jr      ra