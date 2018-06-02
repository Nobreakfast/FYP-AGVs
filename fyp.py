from socket import *
import threading
from tkinter import *

host = '192.168.191.1'
port = 8081
buffsize = 2048
ADDR = (host, port)
point = [0, 1, 2, 3, 4, 5]

setX = 40
setY = 25
index = 25
canvas_width = 450
canvas_height = 300
r = 15
pos = [[0, 5, 7.5, 10, 10, 6, 2.5, 0, 5], [10, 10, 7.5, 5, 0, 0, 0, 5, 5]]
table = [[0, 9, 100, 100, 100, 100, 100, 6, 100],
         [9, 0, 5, 100, 100, 100, 100, 100, 100],
         [100, 5, 0, 6, 100, 100, 100, 100, 3],
         [100, 100, 6, 0, 8, 100, 100, 100, 100],
         [100, 100, 100, 8, 0, 2, 100, 100, 100],
         [100, 100, 100, 100, 2, 0, 4, 100, 9],
         [100, 100, 100, 100, 100, 4, 0, 7, 100],
         [6, 100, 100, 100, 100, 100, 7, 0, 5],
         [100, 100, 3, 100, 100, 9, 100, 5, 0]]
pos1 = pos
m = 1
list = ['list']
color = ['color', 'red', 'blue', 'green', 'yellow']


class app(object):
    def __init__(self):
        self.root = Tk()
        self.root.title("FYP AGV")
        self.map_ = Canvas(self.root, width=canvas_width, height=canvas_height)
        self.fm = Frame(self.root)
        self.fmS = Frame(self.root)
        self.fm1 = Frame(self.fm)
        self.fm2 = Frame(self.fm)
        self.var_ID = IntVar()
        self.var_dest = StringVar()
        self.var_l_S = StringVar()
        for i in range(0, 9):
            pos1[0][i] = pos[0][i] * setX + r
            pos1[1][i] = pos[1][i] * setY + r
            self.map_.create_oval(pos[0][i] - r, pos[1][i] - r, pos[0][i] + r, pos[1][i] + r, fill='black')
        for i in range(0, 9):
            for j in range(0, 9):
                if table[i][j] < 100 and table[i][j] > 0:
                    self.map_.create_line(pos[0][i], pos[1][i], pos[0][j], pos[1][j])

        Label(self.fm1, text='IP : ' + str(host) + ' ' + 'Port : ' + str(port)).pack(side=LEFT)
        Label(self.fm1, text='Port : ').pack(side=LEFT)
        Button(self.fm1, text='START', command=self.buttonStart).pack(side=LEFT)
        Label(self.fm2, text='ID : ').pack(side=LEFT)
        self.var_ID.initialize('1')
        Entry(self.fm2, width=3, textvariable=self.var_ID).pack(side=LEFT)
        Label(self.fm2, text='Destination : ').pack(side=LEFT)
        Entry(self.fm2, width=3, textvariable=self.var_dest).pack(side=LEFT)
        Button(self.fm2, text='Send', command=self.buttonSend).pack(side=LEFT)
        Label(self.fmS, text='Hello!', textvariable=self.var_l_S).pack()

        self.map_.pack(side=TOP)
        self.fm1.pack(side=TOP, fill=BOTH, expand=NO)
        self.fm2.pack(side=TOP, fill=BOTH, expand=NO)
        self.fm.pack(side=LEFT)
        self.fmS.pack(side=LEFT)

    def buttonStart(self):
        global list
        global m
        try:
            carServer = socket(AF_INET, SOCK_STREAM)
            carServer.bind(ADDR)
            carServer.listen(3)
            cc, addr = carServer.accept()
            print("Connection from : ", addr)
            car1 = carThread(cc, addr)
            list.append(car1)
            list[m].start()
            m = m + 1
        except:
            pass

    def buttonSend(self):
        global list
        try:
            id = self.var_ID.get()
            dest = self.var_dest.get()
            print(int(id), int(dest))
            list[id].cc.send(dest.encode())
        except:
            pass
        self.var_ID.initialize('1')
        self.var_dest.initialize('')


class car_info():
    def __init__(self, ID, fun, pre, next, dest):
        self.ID = ID
        self.fun = fun
        self.pre = pre
        self.next = next
        self.dest = dest

    def MessDe(self, Mess):
        self.ID = Mess[1]
        self.fun = Mess[2]
        self.pre = Mess[3]
        self.next = Mess[4]
        return

    def dest_in(self):
        print('Plz enter a destination')
        pos = 0
        while pos in point:
            if pos not in point:
                print('Wrong Destination!')
            pos = input('Plz enter:')
            print('Destination is setting to : ', pos)
        self.dest = pos
        return self.dest

    def print_(self):
        print('ID   :', self.ID, 'fun  :', self.fun, 'pre  :', self.pre, 'dest :', self.next)


class carThread(threading.Thread):
    def __init__(self, cc, addr):
        threading.Thread.__init__(self)
        self.flag = 0
        self.cc = cc
        self.addr = addr
        self.info = car_info(0, 0, 0, 0, 0)

    def run(self):
        global gui
        while True:
            data = self.cc.recv(buffsize).decode()
            if data[0] == '#':
                self.info.MessDe(data)
                self.id = self.info.ID
                self._update(int(self.info.pre), int(self.info.next), int(self.info.ID))
            if data[0] == '*':
                pos_ = self.info.dest_in()
                self.cc.send(pos_.encode())
            if data[0] == '$':
                self.flag = 1
                print('close')
            self.info.print_()
            if self.flag == 1:
                break
        self.cc.close()

    def _update(self, point1, point2, ID):
        global gui
        try:
            gui.map_.delete(self.s)
        except:
            pass
        posx = (pos[0][point1] + pos[0][point2]) / 2
        posy = (pos[1][point1] + pos[1][point2]) / 2
        self.s = gui.map_.create_oval(posx - ID * 5, posy - ID * 5, posx - ID * 5 + 10, posy - ID * 5 + 10,
                                      fill=color[ID])


def main():
    global gui
    gui = app()
    mainloop()


if __name__ == "__main__":
    main()
