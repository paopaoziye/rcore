	.section .text
	.globl _start
	.type _start,@function

_start:
	li		t0,	0x100
	slli	t0,	t0, 20
	li		t1,	0x200
	slli	t1,	t1, 4
	add     t0, t0, t1
	li		t1,	'H'
	sb		t1, 0(t0)
	li		t1,	'i'
	sb		t1, 0(t0)
	li		t1,	'!'
	sb		t1, 0(t0)

_loop:
	j		_loop

    .end