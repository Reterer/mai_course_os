import random as rnd

seed = 42
min_number = -1000000
max_number =  1000000

n1 = 100
m1 = 100
n2 = 100
m2 = 100

test_file_name = "100x100.t"

rnd.seed(seed)

def generate_complex():
    return "{:.2f} {:+.2f}i".format(rnd.uniform(min_number, max_number), rnd.uniform(min_number, max_number))

def generate_matrix_and_write(flie, n, m):
    matrix = [[generate_complex() for _ in range(m)] for _ in range(n)]
    file.write(f"{n} {m}\n")
    file.write('\n'.join(map(lambda x: ' '.join(x) ,matrix)) + '\n')

with open(test_file_name, 'w') as file:
    generate_matrix_and_write(file, n1, m1)
    generate_matrix_and_write(file, n2, m2)
