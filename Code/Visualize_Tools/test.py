class Test():
    def __init__(self, nop):
        self.nop = nop

    def log(self):
        print(self.nop)

class Nop():
    def __init__(self, x):
        self.x = x

    def final(self):
        print(self.x)


nop = Nop(12)
test = Test(nop)
test.nop.final()
print(test.nop.x)