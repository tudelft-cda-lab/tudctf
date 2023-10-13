user_code = input()

for char in user_code:
    if char in "()":
        print("No parentheses!")
        exit()
    
    if char == "\n":
        print("No newlines!")
        exit()

    if char == '"':
        print("No double quotations!")
        exit()

    if char in "#:{}`><,\\":
        print("No weird symbols!")
        exit()

code = f"""def print_round(round):
    print('that was round', round)
    return round == 1 or round == 2 or round == 3 or round == 4

def get_separator():
    sep = "_"
    for _ in (1, 2, 3, 4):
        sep += "-~-_"
    return sep

for round in (1, 2):
    separator = get_separator()
    print(separator)

    {user_code}

    valid_round = print_round(round)
"""

__globals = {"__builtins__": None, "print": print}
exec(code, __globals)
