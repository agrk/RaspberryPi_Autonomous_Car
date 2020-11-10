import RPi.GPIO as GPIO          
from time import sleep

import time

in1 = 24
in2 = 23
en = 25

in3 = 22
in4 = 27
en2 = 17


temp1=1
temp2=1

GPIO.setmode(GPIO.BCM)
GPIO.setup(in1,GPIO.OUT)
GPIO.setup(in2,GPIO.OUT)
GPIO.setup(in3,GPIO.OUT)
GPIO.setup(in4,GPIO.OUT)

GPIO.setup(en,GPIO.OUT)
GPIO.setup(en2,GPIO.OUT)

GPIO.output(in1,GPIO.LOW)
GPIO.output(in2,GPIO.LOW)
GPIO.output(in3,GPIO.HIGH)
GPIO.output(in4,GPIO.HIGH)

p=GPIO.PWM(en,1000)
p2=GPIO.PWM(en2,1000)

p.start(70)
p2.start(70)

print("\n")
print("The default speed & direction of motor is LOW & Forward.....")
print("r-run s-stop f-forward b-backward ri-turn right le-turn left l-low m-medium h-high e-exit")
print("\n")    

while(1):

    x=raw_input()
    
    if x=='r':
        print("run")
        if(temp1==1 and temp2==1):
         GPIO.output(in1,GPIO.HIGH)
         GPIO.output(in2,GPIO.LOW)
         GPIO.output(in3,GPIO.HIGH)
         GPIO.output(in4,GPIO.LOW)
         print("forward")
         x='z'
        else:
         GPIO.output(in1,GPIO.LOW)
         GPIO.output(in2,GPIO.HIGH)
         GPIO.output(in3,GPIO.LOW)
         GPIO.output(in4,GPIO.HIGH)
         print("backward")
         x='z'


    elif x=='s':
        print("stop")
        GPIO.output(in1,GPIO.LOW)
        GPIO.output(in2,GPIO.LOW)
        GPIO.output(in3,GPIO.LOW)
        GPIO.output(in4,GPIO.LOW)
        x='z'

    elif x=='f':
        print("forward")
        GPIO.output(in1,GPIO.HIGH)
        GPIO.output(in2,GPIO.LOW)
        GPIO.output(in3,GPIO.HIGH)
        GPIO.output(in4,GPIO.LOW)
        temp1=1
        temp2=1
        x='z'

    elif x=='b':
        print("backward")
        GPIO.output(in1,GPIO.LOW)
        GPIO.output(in2,GPIO.HIGH)
        GPIO.output(in3,GPIO.LOW)
        GPIO.output(in4,GPIO.HIGH)
       
        x='z'
    
    elif x=='ri':
        print("turn right")
        GPIO.output(in1,GPIO.LOW)
        GPIO.output(in2,GPIO.HIGH)
        GPIO.output(in3,GPIO.HIGH)
        GPIO.output(in4,GPIO.LOW)
       
        p.ChangeDutyCycle(100)
        p2.ChangeDutyCycle(75)

        x='z'
  


    elif x=='le':
        
        print("turn left")
        GPIO.output(in1,GPIO.HIGH)
        GPIO.output(in2,GPIO.LOW)
        GPIO.output(in3,GPIO.LOW)
        GPIO.output(in4,GPIO.HIGH)
        temp1=1
        temp2=0
         
        p.ChangeDutyCycle(75)
        p2.ChangeDutyCycle(100)
        x='z'
    
        
    

    elif x=='l':
        print("low")
        p.ChangeDutyCycle(50)
        p2.ChangeDutyCycle(50)
        x='z'

    elif x=='m':
        print("medium")
        p.ChangeDutyCycle(75)
        p2.ChangeDutyCycle(75)
        x='z'

    elif x=='h':
        print("high")
        p.ChangeDutyCycle(100)
        p2.ChangeDutyCycle(100)
        x='z'
     
    
    elif x=='e':
        GPIO.cleanup()
        print("GPIO Clean up")
        break
    
    else:
        print("<<<  wrong data  >>>")
        print("please enter the defined data to continue.....")
