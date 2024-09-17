bits 16

mov [si - 300], cx
mov ax, [bx + di - 37]
mov dx, [bx - 32]
mov [si + 300], cx
mov ax, [bx + di + 37]
mov dx, [bx + 32]
