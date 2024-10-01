import math

fileRead = 'test.txt'
fileWrite = 'robot'

fileW = open(fileWrite, "w")

def writeFile(file_path, msg):
    file = open(file_path, "w")
    file.write(msg)
    #file.write(msg.get_str())
    file.close()

class Robot:
    cnt = 1
    Xtcp = 1096
    Ytcp = 0
    Ztcp = 1109
    def __init__(self): 
        self.items = []

    def calculate(self, x, y, z, r, p, w):
        results = []
        a = r * math.pi / 180;
        b = (90+p) * math.pi / 180;
        c = w * math.pi / 180;

        ca = math.cos(a);
        sa = math.sin(a);
        cb = math.cos(b);
        sb = math.sin(b);
        cc = math.cos(c);
        sc = math.sin(c);

        # Tính Nx, Ny, Nz, Ox, Oy, Oz, Ax, Ay, Az
        Nx = cb * cc;
        Ny = sa * sb * cc + ca * sc;
        Nz = (-ca * sb * cc + sa * sc);
        Ox = -cb * sc;
        Oy = -sa * sb * sc + ca * cc;
        Oz = (ca * sb * sc + sa * cc);
        Ax = sb;
        Ay = -sa * cb;
        Az = ca * cb;

        # Giá trị Px, Py, Pz sau khi tính toán với tcp (giả sử Xtcp, Ytcp, Ztcp là các tham số công cụ)
        Px = x - (cb * cc * self.Xtcp + (-cb * sc) * self.Ytcp + (sb * self.Ztcp));
        Py = y - ((sa * sb * cc + ca * sc) * self.Xtcp + (-sa * sb * sc + ca * cc) * self.Ytcp + (-sa * cb) * self.Ztcp);
        Pz = z - ((-ca * sb * cc + sa * sc) * self.Xtcp + (ca * sb * sc + sa * cc) * self.Ytcp + (ca * cb) * self.Ztcp);

        results.append([round(Nx, 10), round(Ny, 10), round(Nz, 10), round(Ox, 10), round(Oy, 10), round(Oz, 10), round(Ax, 10), round(Ay, 10), round(Az, 10), round(Px, 10), round(Py, 10), round(Pz, 10)])
        return results

    def set_param(self, type, V, Nx, Ny, Nz, Ox, Oy, Oz, Ax, Ay, Az, Px, Py, Pz): 
        self.items.append([type, V, Nx, Ny, Nz, Ox, Oy, Oz, Ax, Ay, Az, Px, Py, Pz])
    
    def set_param(self, type, V, coordinates):
        arr = []
        for item in coordinates:
            arr.extend(item)
        if(type == 'MOVL'):
            msg = f"{type} VL={V:.1f} PL=1 ACC=0.0 DEC=0 TOOL=4 BASE=0 USE=1 COUNT=0 Nx={arr[0]:.10f} Ny={arr[1]:.10f} Nz={arr[2]:.10f} Ox={arr[3]:.10f} Oy={arr[4]:.10f} Oz={arr[5]:.10f} Ax={arr[6]:.10f} Ay={arr[7]:.10f} Az={arr[8]:.10f} Px={arr[9]:.10f} Py={arr[10]:.10f} Pz={arr[11]:.10f} N{self.cnt}\n"
        elif(type == 'MOVJ'):
            msg = f"{type} VJ={V:.1f} PL=1 ACC=0.0 DEC=0 TOOL=4 BASE=0 USE=1 COUNT=0 Nx={arr[0]:.10f} Ny={arr[1]:.10f} Nz={arr[2]:.10f} Ox={arr[3]:.10f} Oy={arr[4]:.10f} Oz={arr[5]:.10f} Ax={arr[6]:.10f} Ay={arr[7]:.10f} Az={arr[8]:.10f} Px={arr[9]:.10f} Py={arr[10]:.10f} Pz={arr[11]:.10f} N{self.cnt}\n"

        self.cnt = self.cnt + 1
        self.items.append(msg)
    
    def get_OneCommand(self, type, V, coordinates):
        arr = []
        for item in coordinates:
            arr.extend(item)
        if(type == 'MOVL'):
            msg = f"{type} VL={V:.1f} PL=1 ACC=0.0 DEC=0 TOOL=4 BASE=0 USE=1 COUNT=0 Nx={arr[0]:.10f} Ny={arr[1]:.10f} Nz={arr[2]:.10f} Ox={arr[3]:.10f} Oy={arr[4]:.10f} Oz={arr[5]:.10f} Ax={arr[6]:.10f} Ay={arr[7]:.10f} Az={arr[8]:.10f} Px={arr[9]:.10f} Py={arr[10]:.10f} Pz={arr[11]:.10f} N{self.cnt}\n"
        elif(type == 'MOVJ'):
            msg = f"{type} VJ={V:.1f} PL=1 ACC=0.0 DEC=0 TOOL=4 BASE=0 USE=1 COUNT=0 Nx={arr[0]:.10f} Ny={arr[1]:.10f} Nz={arr[2]:.10f} Ox={arr[3]:.10f} Oy={arr[4]:.10f} Oz={arr[5]:.10f} Ax={arr[6]:.10f} Ay={arr[7]:.10f} Az={arr[8]:.10f} Px={arr[9]:.10f} Py={arr[10]:.10f} Pz={arr[11]:.10f} N{self.cnt}\n"
        self.cnt = self.cnt + 1
        return msg

    def get_FullStr(self):
        return " ".join(self.items)

def readFile(file_path, msg):
    with open(file_path, 'r') as file:
        for line in file:
            txt = line.split()
            if txt[0] == 'MOVL' or txt[0] == 'MOVJ':
                _, value = txt[1].split('=')
                coordinates = msg.calculate(float(txt[2]), float(txt[3]), float(txt[4]), float(txt[5]), float(txt[6]), float(txt[7]))
                tmp = msg.get_OneCommand(txt[0], float(value), coordinates)
                fileW.write(tmp)
            else:
                fileW.write(line)
    
if __name__ == "__main__":
    msg = Robot()
    readFile(fileRead, msg)

    fileW.close()
    # writeFile(fileWrite, msg)
    # print(msg.get_FullStr())