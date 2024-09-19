;NOTE: REMOVED IMEDIATE TESTS. ADDED DIRECT SIZES AND SIGNED DISPLACEMENTS
bits 16

; Signed displacements
mov ax, [bx + di - 37]
mov [si - 300], cx
mov dx, [bx - 32]

; Direct address
mov bp, [5]
mov bx, [3458]


; Register-to-register
mov si, bx
mov dh, al

; Source address calculation
mov al, [bx + si]
mov bx, [bp + di]
mov dx, [bp]

; Source address calculation plus 8-bit displacement
mov ah, [bx + si + 4]

; Source address calculation plus 16-bit displacement
mov al, [bx + si + 4999]

; Dest address calculation
mov [bx + di], cx
mov [bp + si], cl
mov [bp], ch



