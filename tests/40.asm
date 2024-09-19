bits 16

; Explicit sizes
mov [di + 901], word 347
mov [bp + di], byte 7

; 8-bit immediate-to-register
mov cl, 12
mov ch, -12

; 16-bit immediate-to-register
mov cx, 12
mov cx, -12
mov dx, 3948
mov dx, -3948
