# solution: 2768593938126314414713327
boardtemplate ← [8 0 4 7 5 3 9 6 1 0 3 0 1 9 0 2 4 0 1 5 0 6 2 4 0 7 8 0 4 2 0 7 5 1 0 6 5 0 8 0 4 0 7 9 0 6 7 3 8 0 9 5 2 0 2 6 5 0 3 7 8 0 9 0 9 0 5 8 0 6 0 2 0 8 1 9 6 0 4 5 0]

# |1.1: Return 1 if the given input is unique 1-9
valid ← ≅⊝.

# |1.1 Return 9-long array for all row validity
rowsvalid ← ≡valid

# |1.1 Return 9-long array for all column validity
columnsvalid ← ≡valid ⍉

# |1.1 Return 9-long array for all 3x3 validities
tbtvalid ← ♭ ≡(≡(valid♭)⍉)↯ 3_3_3_3

# |1.1 Return 1 if all elements are 1
allone ← ≅[1]⊝

# |2.1 Substitute all zeros in the second argument with the values in the top argument
subst ← /(|2 ⊂∩!)♭∵⊂∶,⊜□≠,0∶

# Read inputs, convert to ascii, assert length and 1-9.
lencheck ← ⍤"Wrong length!"=25⧻
charcheck ← ⍤"Invalid chars!"=0/+⊂⊃(<@1)(>@9)

&p "What's the secret code?"

# |0.1 Input as string.
inputstr ← |0 charcheck. lencheck. &sc

# |0.2 Input as string and as digits.
inputs ← -@0.inputstr

board ← ↯ 9_9 subst inputs boardtemplate

allone ⊂⊂⊃⊃rowsvalid columnsvalid tbtvalid board
?(&p$"That's correct! The flag is TUDCTF{_}!";)(;;&p"That's wrong.")
