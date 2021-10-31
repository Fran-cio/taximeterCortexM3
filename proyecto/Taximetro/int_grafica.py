from tkinter import *
import time


DEVICE_FILE = "/dev/ttyUSB0" #check nombre /dev/usb

MODO_O = 'O'
MODO_L = 'L'
MODO_P = 'P'

root = Tk()

screen_ancho = root.winfo_screenwidth()
screen_largo = root.winfo_screenheight()

def set_interface():    

    global distance_now, hora, fecha,tarifa, state_ocupado, state_libre, state_stop

    root.title("TAXIMETRO")
    frame = Canvas(root,width=screen_ancho,height=screen_largo,borderwidth=30,background = "black",relief="ridge")
    frame.pack(expand=YES, fill=BOTH)   
    screen_width=screen_ancho/2000
    screen_height=screen_largo/1000

    rect=frame.create_rectangle(screen_width*1350,250,70,80,width=15,outline="grey")
    frame.move(rect, screen_width*450, screen_height*400)

    Label(frame, text= "Tarifa", fg="red", bg="black", font="times 24 bold").place(x=screen_width*200, y=screen_height*200)

    Label(frame, text="TAXIMETRO - Grupo 1", fg="red", bg="black", font="times 24 bold").place(x=screen_width*800, y=screen_height*50)

    distance = Label(frame, text= "Distancia", fg="red", bg="black", font="times 24 bold")
    distance.place(x=screen_width*800, y=screen_height*200)

    distance_now = Label(frame,text= "100 KM",fg="red", bg="black",font="times 24 bold")
    distance_now.place(x=screen_width*800,y=screen_height*300)

    monto = Label(frame, text= "Monto", fg="red", bg="black", font="times 24 bold")
    monto.place(x=screen_width*500, y=screen_height*200)

    hora=Label(frame,font=("times",15,"bold"))
    hora.place(x=screen_width*40, y=screen_height*880)

    fecha=Label(frame,font=("times",15,"bold"))
    fecha.place(x=screen_width*40, y=screen_height*840)

    tarifa = Label(frame,text= "$0000",fg="red", bg="black", font="times 24 bold")
    tarifa.place(x=screen_width*500,y=screen_height*300)

    tarifa_now = Label(frame,text= "1",fg="red", bg="black", font="times 24 bold")
    tarifa_now.place(x=screen_width*270,y=screen_height*300)

    state_ocupado = Label(frame,text= "OCUPADO",fg="red", bg="black", font="times 14 bold")
    state_ocupado.place(x=screen_width*550,y=screen_height*550)

    state_libre = Label(frame,text= "LIBRE",fg="white", bg="black", font="times 14 bold")
    state_libre.place(x=screen_width*1650,y=screen_height*550)

    state_stop = Label(frame,text= "STOP",fg="white", bg="black", font="times 14 bold")
    state_stop.place(x=screen_width*1150,y=screen_height*550)

    frame.image_tax= PhotoImage(file="./Img/taxi.png",width=int(screen_width*298), height=int(screen_height*300))
    Label(frame, image=frame.image_tax, bg = 'black').place(x=screen_width*1570,y=screen_height*650)

    frame.btn_start = PhotoImage(file="./Img/bRojo.png",width=int(screen_width*298), height=int(screen_height*300))
    Label(frame, image=frame.btn_start, text='Start', font="times 20 bold",bg = 'black', compound='center').place(x=screen_width*502,y=screen_height*650)

    frame.btn_stop = PhotoImage(file="Img/bRojo.png",width=int(screen_width*298), height=int(screen_height*300))
    Label(frame, image=frame.btn_stop, text='Stop', font="times 20 bold",bg = 'black', compound='center').place(x=screen_width*802,y=screen_height*650)

    frame.btn_reset = PhotoImage(file="Img/bRojo.png",width=int(screen_width*298), height=int(screen_height*300))
    Label(frame, image=frame.btn_reset, text='Reset', font="times 20 bold",bg = 'black', compound='center').place(x=screen_width*1102,y=screen_height*650)

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


set_interface()

times_hora()
times_fecha()
#set_monto()


mainloop()
