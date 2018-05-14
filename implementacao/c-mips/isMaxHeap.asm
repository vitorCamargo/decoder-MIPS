.data
	v: .space 0
	msg: .asciiz "Digite a posição a ser verificada: "
.text
		j main
		
isMaxHeap:	nop
		addi $sp, $sp, -32
		sw $ra, 28($sp)
		sw $fp, 24($sp)
		move $fp, $sp

		sw $a1, 36($fp)		# Armazena n na memória
		sw $a2, 40($fp)		# Armazena i na memória
		
		lw $t0, 36($fp)		# Armzena em $t0 o valor n
		srl $t0, $t0, 1		# Faz n/2
		lw $t1, 40($fp)		# Armazena em $t1 o valor de i
		
		bgt $t0, $t1, else_if	# Se (n/2) > i -> vai para else_if
		addi $v0, $zero, 1	# Armazena resultado em $v0
		j desaloca

else_if:	lw $t1, 40($fp)		# Armazena em $t1 o valor de i
		sll $t1, $t1, 2		# Caarrega em $t1 a posição i correspondente na memória
		lw $t1, v($t1)		# Carrega em $t1 o valor de v[i]
		
		lw $t2, 40($fp)		# Armazena em $t2 o valor de i
		sll $t2, $t2, 1		# i*2
		addi $t2, $t2, 1	# (i*2)+1
		sll $t2, $t2, 2		# Caarrega em $t2 a posição i correspondente na memória
		lw $t2, v($t2)		# Carrega em $t2 o valor de v[(i*2)+1]
		
		lw $t3, 40($fp)		# Armazena em $t3 o valor de i
		sll $t3, $t3, 1		# i*2
		addi $t3, $t3, 2	# (i*2)+2
		sll $t3, $t3, 2		# Carrega em $t2 a posição i correspondente na memória
		lw $t3, v($t3)		# Carrega em $t2 o valor de v[(i*2)+2]
		
		bge $t2, $t1, else	# Se v[(i*2)+1] >= v[i] -> vai para else
		bge $t3, $t1, else	# Se v[(i*2)+2] >= v[i] -> vai para else
		
		lw $t0, 40($fp)		# Carrega em $t0 o i atual
		addi $a2, $t0, 1	# Armezana valor i+1
		lw $t0, 36($fp)		# Carrega em $t0 o n atual
		addi $a1, $t0, 0	# Armazena valor n
		
		jal isMaxHeap
		
		j desaloca

else:		nop
		addi $v0, $zero, 0	# Armazena resultado em $v0

desaloca:	nop
		move $sp, $fp
		lw $ra, 28($sp)
		lw $fp, 24($sp)
		addiu $sp, $sp, 32
		jr $ra
		
main:		nop
		addi $sp, $sp, -32
		sw $ra, 28($sp)
		sw $fp, 24($sp)
		move $fp, $sp
		
		addi $t0, $0, 0		# $t0 = 0
		addi $t1, $0, 0		# $t1 = 0 (memória)
		addi $s0, $0, 7		# $s0 = 7 (limite)
	
		li $v0, 9
		li $a0, 28		# $a0 = number of bytes to allocate (7*4)
		syscall
	
		move $t2, $v0
	
for:		beq $s0, $t0, done
		
		li $v0, 5
		syscall
	
		sw $v0, v($t1)
	
		addi $t0, $t0, 1
		addi $t1, $t1, 4
		j for

done:		# printf: "Digite a posição a ser verificada: "
		li $v0, 4
		la $a0, msg
		syscall
		
		li $v0, 5
		syscall
		
		move $t3, $v0		# Armazena resultado da pergunta 
		
		addi $a2, $t3, 0	# Armezana valor i
		addi $a1, $s0, 0 	# Armazena valor n
		jal isMaxHeap		# Chama função
		nop

		move $s0, $v0		# Carrega resultado da função em $s0
		
		li  $v0, 1           	# Imprime resultado
    		add $a0, $s0, $zero  	# Carrega valor para mostrar
    		syscall
		
		move $sp, $fp
		lw $ra, 28($sp)
		lw $fp, 24($sp)
		addiu $sp, $sp, 32
		
		li $v0, 10
		syscall