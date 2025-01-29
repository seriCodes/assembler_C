mcro firstMacro
clr r3
prn r4
cmp #7 , #99
mcroend
.data 58 , 99
first_label:
notIntru r5
clr r2
prn r2
.string "zxcv"
SecondMacro
sub r3, first_label
middle_label: sub r1 , r6
cmp #7, #99
jsr &last_label
add #213, r4
sub #990, r5
mcro SecondMacro
clr r3
prn r4
cmp #7 , #99
mcroend
last_label:
red middle_label
bne &first_label
Data_Label: .data 1,-52
firstMacro
jmp Data_Label
.data 99,55