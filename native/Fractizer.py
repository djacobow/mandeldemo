#!/usr/local/bin/python3


# You'll need a .so file:
#
# g++ -c -fPIC -O3 fractizer.cpp -o fractizer.o
# g++ -shared fractizer.o -o fractizer.so

import ctypes

class fparams_t(ctypes.Structure):
    _fields_ = [
        ('max_iters', ctypes.c_ushort),
        ('escape_val', ctypes.c_double),
        ('x_min', ctypes.c_double),
        ('x_max', ctypes.c_double),
        ('y_min', ctypes.c_double),
        ('y_max', ctypes.c_double),
        ('x_pels', ctypes.c_ushort),
        ('y_pels', ctypes.c_ushort),
        ('x_tile', ctypes.c_ushort),
        ('y_tile', ctypes.c_ushort),
        ('type', ctypes.c_ubyte),
        ('do_julia', ctypes.c_ubyte),
        ('jx', ctypes.c_double),
        ('jy', ctypes.c_double),
        ('algo',ctypes.c_void_p),
]


class Fractizer(object):
    def __init__(self):
        self.fr = ctypes.cdll.LoadLibrary('./fractizer.so')
        self.params = fparams_t()
        self.fr.set_default_params(ctypes.byref(self.params))

    def getParams(self):
        return self.params

    def showParams(self):
        self.fr.showParams(ctypes.byref(self.params))

    def dumpParams(self):
        print('max_iters', self.params.max_iters)
        print('escape_val', self.params.escape_val)
        print('x_min', self.params.x_min)
        print('x_max', self.params.x_max)
        print('y_min', self.params.y_min)
        print('y_max', self.params.x_max)
        print('x_pels', self.params.x_pels)
        print('y_pels', self.params.y_pels)
        print('x_tile', self.params.x_tile)
        print('y_tile', self.params.y_tile)
        print('type', self.params.type)
        print('do_julia', self.params.do_julia)
        print('jx', self.params.jx)
        print('jy', self.params.jy)
        print('algo',self.params.algo)

    def compute(self):
        output_len = self.params.x_pels * self.params.y_pels
        output_ary_t = ctypes.c_ushort * output_len
        output_ary = output_ary_t()
        poutput_ary = ctypes.pointer(output_ary)
        self.fr.generate_fractal(ctypes.byref(self.params),poutput_ary)
        return output_ary

    def showResult(self,ary):
        olines = [];
        for j in range(self.params.y_pels):
            x = [ary[i + j*self.params.x_pels] for i in range(self.params.y_pels)]
            y = ['{0:4}'.format(q) for q in x]
            olines.append(' '.join(y))
        return '\n'.join(olines)



if __name__ == '__main__':

    f = Fractizer()
    f.getParams().x_pels = 20
    f.getParams().y_pels = 20
    f.showParams()
    result = f.compute()
    print(f.showResult(result))



