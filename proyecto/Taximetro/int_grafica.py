from tkinter import *
import time
from PIL import Image
from PIL import ImageTk

import serial
import sys
 
puerto = '/dev/ttyUSB0'
#-------------------------
#-- Abrir puerto serie
#-------------------------
try:
        sg = serial.Serial(puerto, 9600)
        sg.timeout = 1;
 
except serial.SerialException:
        #-- Error al abrir el puerto serie
        sys.stderr.write("Error al abrir el puerto " +str(puerto) +"\n")
        sys.exit(1)
 
#-- Mostrar nombre del dispositivo serie utilizado
print("Puerto: " +str(puerto))
 
#-------------------------
#-- Prueba del puerto
#-------------------------
 
#-- Enviar cadena de pruebas


DEVICE_FILE = "/dev/ttyUSB0" #check nombre /dev/usb

MODO_O = 'O'
MODO_L = 'L'
MODO_P = 'P'

def set_interface():    

    global distance_now, hora, fecha,tarifa, state_ocupado, state_libre, state_stop

    root.title("TAXIMETRO")
    frame = Canvas(root,width=screen_ancho,height=screen_largo,borderwidth=30,background = "black",relief="ridge")
    frame.pack(expand=YES, fill=BOTH)   
    screen_width=screen_ancho/2000
    screen_height=screen_largo/1000

    rect=frame.create_rectangle(screen_width*1350,screen_height*250,screen_width*70,screen_height*80,width=screen_width*15,outline="grey")
    frame.move(rect, screen_width*450, screen_height*400)

    Label(frame, text= "Tarifa", fg="red", bg="black", font="times 24 bold").place(x=screen_width*200, y=screen_height*200)

    Label(frame, text="TAXIMETRO - Grupo 1", fg="red", bg="black", font="times 24 bold").place(x=screen_width*800, y=screen_height*50)

    distance = Label(frame, text= "Distancia", fg="red", bg="black", font="times 24 bold")
    distance.place(x=screen_width*800, y=screen_height*200)

    distance_now = Label(frame,text= " 0000 m",fg="red", bg="black",font="times 24 bold")
    distance_now.place(x=screen_width*800,y=screen_height*300)

    monto = Label(frame, text= "Monto", fg="red", bg="black", font="times 24 bold")
    monto.place(x=screen_width*500, y=screen_height*200)

    hora=Label(frame,font=("times",15,"bold"))
    hora.place(x=screen_width*50, y=screen_height*840)

    fecha=Label(frame,font=("times",15,"bold"))
    fecha.place(x=screen_width*50, y=screen_height*800)

    tarifa = Label(frame,text= "$0000",fg="red", bg="black", font="times 24 bold")
    tarifa.place(x=screen_width*500,y=screen_height*300)

    tarifa_now = Label(frame,text= "1",fg="red", bg="black", font="times 24 bold")
    tarifa_now.place(x=screen_width*270,y=screen_height*300)

    state_ocupado = Label(frame,text= "OCUPADO",fg="red", bg="black", font="times 14 bold")
    state_ocupado.place(x=screen_width*550,y=screen_height*550)

    state_libre = Label(frame,text= "LIBRE",fg="white", bg="black", font="times 14 bold")
    state_libre.place(x=screen_width*1650,y=screen_height*550)

    state_stop = Label(frame,text= "PARADO",fg="white", bg="black", font="times 14 bold")
    state_stop.place(x=screen_width*1150,y=screen_height*550)

    image = Image.open("./Img/taxi.png")
    resize_image = image.resize((int(200*screen_width), int(200*screen_height)))
    frame.image_tax = ImageTk.PhotoImage(resize_image)
    Label(frame, image=frame.image_tax, bg = 'black').place(x=screen_width*1570,y=screen_height*660)

    image = Image.open("./Img/bRojo.png")
    resize_image = image.resize((int(225*screen_width), int(220*screen_height)))
    frame.btn_start = ImageTk.PhotoImage(resize_image)
    Label(frame, image=frame.btn_start, text='LIBRE', font="times 20 bold",bg = 'black', compound='center').place(x=screen_width*502,y=screen_height*655)

    Label(frame, image=frame.btn_start, text='OCUPADO', font="times 13 bold",bg = 'black', compound='center').place(x=screen_width*802,y=screen_height*655)

    Label(frame, image=frame.btn_start, text='PARADO', font="times 15 bold",bg = 'black', compound='center').place(x=screen_width*1102,y=screen_height*655)

def times_fecha():

    fecha.config(text=time.strftime("Fecha: %m/%d/%Y"), bg="black",fg="red",font="Arial 15 bold")
    fecha.after(200,times_fecha)

def times_hora():   
    hora.config(text=time.strftime("Hora: %H:%M:%S"), bg="black",fg="red",font="Arial 15 bold")
    hora.after(200,times_hora)

def get_monto(value):
	string = ' '
	string = value[4]+(value[5])+value[6]+value[7]
	return string
def get_distance(value):
	string = ' '
	string = value[9]+value[10]+value[11]+value[12]+value[13]
	return string

def set_data():

    datosASCII = sg.read(15) #Devuelve b
	
    i=0

    value = ""
    for i in datosASCII:
        value = value + chr(i)

    if (value[1] == MODO_O):        
        tarifa.config(text=("$ "+get_monto(value)), bg="black",fg="red",font="Arial 15 bold")
        distance_now.config(text=(" "+get_distance(value)+" m"), bg="black",fg="red",font="Arial 15 bold") 
        state_ocupado.config(fg="red")   
        state_stop.config(fg="white") 
        state_libre.config(fg="white") 

    if (value[1] == MODO_L):
        tarifa.config(text="$ 0000", bg="black",fg="red",font="Arial 15 bold")
        distance_now.config(text=(" 00000 m"), bg="black",fg="red",font="Arial 15 bold") 
        state_ocupado.config(fg="white")   
        state_stop.config(fg="white") 
        state_libre.config(fg="red")          

    if (value[1] == MODO_P):
        aux=value.split("$")
        tarifa.config(text=("$ "+get_monto(value)), bg="black",fg="red",font="Arial 15 bold")
        distance_now.config(text=(" "+get_distance(value)+" m"), bg="black",fg="red",font="Arial 15 bold") 
        state_ocupado.config(fg="white")   
        state_stop.config(fg="red") 
        state_libre.config(fg="white") 

    tarifa.after(10,set_data)   
 	   
root = Tk()

screen_ancho = root.winfo_screenwidth()
screen_largo = root.winfo_screenheight()

set_interface()

times_hora()
times_fecha()
set_data()


mainloop()
