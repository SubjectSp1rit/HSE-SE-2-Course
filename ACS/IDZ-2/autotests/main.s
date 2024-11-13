# автотесты

.include "macrolib.s"

.data
# Строки
separator_str: .asciz ": "
success_str:   .asciz " - Passed"
fail_str:      .asciz " - Failed"
newline_str:   .asciz "\n"

# Эталонное значение π, выровненный по границе 8 байт для double
.align 3                
pi_ref: .double 3.141592653589793

# Массив тестовых значений N, выровненный по границе 4 байта
.align 2
test_values:
	.word 1
    	.word 10
    	.word 50
    	.word 100
   	.word 200
	.word 500
	.word 700
  	.word 1000
   	.word 1500
  	.word 2000	
 
.text
.globl main
main:
    	li t2 0 	# Счетчик индекса массива
    	li t1 9		# Кол-во тестовых значений

test_loop:
	# Проверка, достигли ли конца массива
    	bge   t2 t1 end_program # Если счетчик достиг количества элементов => завершить тестирование

    	# Вычисляем адрес текущего элемента массива
    	la t0 test_values	# Загружаем базовый адрес массива в t0
    	slli t3 t2 2		# Умножаем индекс t2 на 4 для получения смещения
    	add t0 t0 t3		# Получаем адрес текущего элемента массива

    	# Загрузка текущего значения из массива в a0
    	lw a0 0(t0)

    	# Вызов подпрограммы compute_pi
    	compute_pi        	# Вычисляем π при помощи N итераций
    	# Результат находится в fa0 (найденное π)

    	# Загрузка эталонного значения π
    	la t4 pi_ref
    	fld ft1 0(t4)

    	# Вычисление абсолютной разницы между эталонным и найденным π
    	fsub.d ft2 ft1 fa0    	# ft2 = pi_ref - pi_computed
    	fabs.d ft2 ft2         	# ft2 = |pi_ref - pi_computed|

    	# Вычисление относительной ошибки: error = (|π_ref - π_computed| / π_ref) * 100%
    	fdiv.d ft3 ft2 ft1    	# ft3 = ft2 / π_ref
    	li t3 100
    	fcvt.d.w ft4 t3        	# ft4 = 100.0
    	fmul.d ft5 ft3 ft4    	# ft5 = ошибка в процентах

    	# Вывод результатов
    	# Печать N
    	print_int a0

    	# Печать ": "
    	print_mstr separator_str

    	# Печать вычисленного π
    	print_double fa0

    	# Проверка, укладывается ли ошибка в 0.05%
    	# Сравниваем ft5 и 0.05
    	li t3 5                	# t3 = 5
    	fcvt.d.w ft6 t3        	# ft6 = 5.0
    	fdiv.d ft6 ft6 ft4    	# ft6 = 5.0 / 100.0 = 0.05

    	fle.d t4 ft5 ft6      	# t4 = (ft5 <= 0.05)
    	beq t4 zero test_fail 	# Если ошибка больше 0.05% => перейти на test_fail

   	# Печать сообщения об успешном прохождении теста
    	print_mstr success_str
    	j test_next

test_fail:
   	print_mstr fail_str

test_next:
    	print_mstr newline_str

    	# Переход к следующему элементу массива
    	addi t2 t2 1          	# Увеличиваем индекс t2
    	j test_loop             # Переход к следующей итерации цикла

end_program:
    	# Завершение программы
    	exit
