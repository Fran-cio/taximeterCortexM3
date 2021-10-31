from tkinter import *
import time


DEVICE_FILE = "/dev/ttyUSB0" #check nombre /dev/usb

MODO_O = 'O'
MODO_L = 'L'
MODO_P = 'P'


def set_interface():    

    global distance_now, hora, fecha,tarifa, state_ocupado, state_libre, state_stop

    root.title("TAXIMETRO")
    frame = Canvas(root,width=1920,height=1080,borderwidth=30,background = "black",relief="ridge")
    frame.pack(expand=YES, fill=BOTH)   

    rect=frame.create_rectangle(1350,250,70,80,width=15,outline="grey")
    frame.move(rect, 450, 400)

    Label(frame, text= "Tarifa", fg="red", bg="black", font="times 24 bold").place(x=200, y=200)

    Label(frame, text="TAXIMETRO", fg="red", bg="black", font="times 24 bold").place(x=800, y=50)

    distance = Label(frame, text= "Distancia", fg="red", bg="black", font="times 24 bold")
    distance.place(x=800, y=200)

    distance_now = Label(frame,text= "100 KM",fg="red", bg="black",font="times 24 bold")
    distance_now.place(x=800,y=300)

    monto = Label(frame, text= "Monto", fg="red", bg="black", font="times 24 bold")
    monto.place(x=500, y=200)

    hora=Label(frame,font=("times",15,"bold"))
    hora.place(x=40, y=980)

    fecha=Label(frame,font=("times",15,"bold"))
    fecha.place(x=40, y=940)

    tarifa = Label(frame,text= "$0000",fg="red", bg="black", font="times 24 bold")
    tarifa.place(x=500,y=300)

    tarifa_now = Label(frame,text= "1",fg="red", bg="black", font="times 24 bold")
    tarifa_now.place(x=270,y=300)

    state_ocupado = Label(frame,text= "OCUPADO",fg="red", bg="black", font="times 14 bold")
    state_ocupado.place(x=550,y=550)

    state_libre = Label(frame,text= "LIBRE",fg="white", bg="black", font="times 14 bold")
    state_libre.place(x=1650,y=550)

    state_stop = Label(frame,text= "STOP",fg="white", bg="black", font="times 14 bold")
    state_stop.place(x=1150,y=550)

    frame.image_tax= PhotoImage(file="./Img/taxi.png",width=298, height=300)
    Label(frame, image=frame.image_tax, bg = 'black').place(x=1570,y=680)

    frame.btn_start = PhotoImage(file="./Img/bRojo.png",width=298, height=300)
    Label(frame, image=frame.btn_start, text='Start', font="times 20 bold",bg = 'black', compound='center').place(x=502,y=685)

    frame.btn_stop = PhotoImage(file="Img/bRojo.png",width=298, height=300)
    Label(frame, image=frame.btn_stop, text='Stop', font="times 20 bold",bg = 'black', compound='center').place(x=802,y=685)

    frame.btn_reset = PhotoImage(file="Img/bRojo.png",width=298, height=300)
    Label(frame, image=frame.btn_reset, text='Reset', font="times 20 bold",bg = 'black', compound='center').place(x=1102,y=685)

def times_fecha():

    fecha.config(text=time.strftime("Fecha: %m/%d/%Y"), bg="black",fg="red",font="Arial 15 bold")
    fecha.after(200,times_fecha)

def times_hora():   
    hora.config(text=time.strftime("Hora: %H:%M:%S"), bg="black",fg="red",font="Arial 15 bold")
    hora.after(200,times_hora)

def set_monto():
    file = open(DEVICE_FILE, "r")
    value = file.read()
    file.close()

    if (value[1] == MODO_O):
        aux=value.split("$")
        tarifa.config(text=("$ "+aux[1]), bg="black",fg="red",font="Arial 15 bold") 
        state_ocupado.config(fg="red")   
        state_stop.config(fg="white") 
        state_libre.config(fg="white") 

    if (value[1] == MODO_L):
        tarifa.config(text="$ 0000", bg="black",fg="red",font="Arial 15 bold")
        state_ocupado.config(fg="white")   
        state_stop.config(fg="white") 
        state_libre.config(fg="red")          

    if (value[1] == MODO_P):
        aux=value.split("$")
        tarifa.config(text=("$ "+aux[1]), bg="black",fg="red",font="Arial 15 bold")
        state_ocupado.config(fg="white")   
        state_stop.config(fg="red") 
        state_libre.config(fg="white") 

    tarifa.after(10,set_monto)   


root = Tk()

set_interface()

times_hora()
times_fecha()
set_monto()


mainloop()
