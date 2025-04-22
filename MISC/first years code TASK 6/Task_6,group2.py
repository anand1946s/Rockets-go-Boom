import math as mat
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
def max_height(netmass,massfintial,invol,twr,dt):
        gamma=1.4
        intemp=300
        mfac=(gamma-1)/2
        efac=(gamma+1)/(2*(gamma-1))
        specific_R=296.8
        twr=twr
        Mst=netmass
        pressure_ex=1.01325*(10**5)
        pressure_atm=1.01325*(10**5)
        thrust_intial=twr*Mst*g
        isp=60
        pressure=60*(10**5)
        mdot_intial=thrust_intial/(isp*g)
        Mach=2*((pressure_ex/pressure)**((1 - gamma)/gamma) - 1)/(gamma - 1) 
        Ve=Mach*(gamma*R*intemp)**0.5
        diametre= mat.sqrt( (4 / mat.pi) * (mdot_intial * mat.sqrt(intemp)) / (pressure * mat.sqrt(1.4 / specific_R) * 0.5787) )
        at = mat.pi * ((diametre/2)**2)
        #print("area",at)
        dt=dt
        a=(thrust_intial-(Mst*g))/Mst
        v=a*dt
        h=(v*dt)+(0.5*a*(dt**2))
        heights=[0,h]
        #a=0
        #v=0
        T=dt
        #h=0
        mdot=mdot_intial
        thrust=thrust_intial
        

        #print(f"velocity{v},acc={a},mdot={mdot_intial},mach{Mach},h={h}before loop")
        while True:
            if massfintial>0 or massfintial>mdot:# thrust>(Mst*g):
            #print(f"mst={Mst}")
             Mst-=(mdot*dt)
            #print(f"mst={Mst}")
             massfintial-=mdot*dt
             if massfintial >0:
              #print(f"massfinal={massfintial}")
              pressure=(massfintial*R*intemp)/(0.028*invol)
              #print(f"pressure={pressure}")
              #print(f"mdot{mdot}")
              mdot=at*pressure*mat.sqrt(gamma/(intemp*specific_R))*mat.pow((gamma+1)/2,-(gamma+1)/(2*(gamma-1)))
            #print(f"mdot{mdot}")
            #Mach=2*((pressure_ex/pressure)**((1 - gamma)/gamma) - 1)/(gamma - 1) 
              Mach=mat.pow(((2/(gamma-1))*(((pressure_ex/pressure)**((1-gamma)/gamma))-1)),1/2)
            #print("mach",Mach,"mdot",mdot)
              ex_temp=intemp/(1+((gamma-1)/2)*(Mach**2))
              pressure_ex=pressure*mat.pow((1+((gamma-1)/2)*(Mach**2)),(-gamma/(gamma-1)))
              Ve=Mach*((gamma*specific_R*ex_temp)**0.5)
              ae=at*((2/gamma+1)*(1+(mfac*(Mach**2))/Mach))**efac
            #thrust=(mdot*Ve)+((pressure_ex-pressure)*ae)
              thrust=(mdot*Ve)+((pressure_ex-pressure_atm)*ae)
            #print(f"thrust={thrust},Mst={Mst*g},pressure difference={pressure_ex-pressure_atm}")
              a=(thrust-(Mst*g))/Mst
              v+=a*dt
              h=h+(v*dt)
              T+=dt
              isp=thrust/mdot
              twr=thrust/Mach
              heights.append(h)
             else:
                break 
             if  h<0 or h>1000 : 
                    break 
             
             if  h<0 or h>1000 : 
                    break  
            
            else:
                v-=(g*dt)
                h+=v*dt
                thrust=0
                mdot=0
                T+=dt
                if  h<0 or h>1000 : 
                    break      
            #print(f"height={h} massf={massfintial}, thrust={thrust},mdot={mdot*dt}  ,acc={a},velocity={v}") 
        #print(f" maximum height of flight is:{max(heights)}")
        k=max(heights)
        return k


#task 
payload=1.5#kg
isp=60
g=9.81
dismin= 1000.0 # "enter desireble distance in metres:"))
twr= 1.2 #"enter twr:"))
dt= 0.001 # float(input("enter dt:"))
deltaV=(g*2*dismin)**0.5
print(f"deltaV={deltaV}")
#task 2
enginemass=0.25
R=8.3145#si
gamma=1.4
mfac=(gamma-1)/2
efac=(gamma+1)/(2*(gamma-1))
specific_R=296.8
pressure=60*(10**5)
density=2700#kg/m3
zig=276000000
fos=2
intemp=300
massfintial=0
invol=(massfintial*1000*R*intemp)/(28*pressure)
r=((3*invol/(4*mat.pi)))**(1/3)
ttank=(pressure*r*fos)/(4*zig)
outvol=(((ttank+r)**3)*4*mat.pi)/3
netvol=outvol-invol
Masstank=density*netvol
netmass=massfintial+enginemass+payload+Masstank
drymass=enginemass+payload+Masstank
mfac=(gamma-1)/2
while True:
    massfintial+=0.001
    x=deltaV/(isp*g)
    invol=(massfintial*R*intemp)/(0.028*pressure)
    r=(3*invol/(4*mat.pi))**(1/3)
    ttank=(pressure*r*fos)/(4*zig)
    outvol=(((ttank+r)**3)*4*mat.pi)/3
    netvol=outvol-invol
    Masstank=density*netvol
    netmass=massfintial+enginemass+payload+Masstank
    drymass=enginemass+payload+Masstank
    y=mat.log(netmass/drymass)
    w=x-y
    if w>=0.00001:
        continue
    else:
        print(f"intially,mass of the tank={Masstank},netmass={netmass},fuel={massfintial},tic={ttank},invol={invol},outvol={outvol}")
        break

massfintial_2=massfintial    
''''#task3
twr=2
Mst=netmass
g=9.81
thrust=twr*Mst*g
deltaV=140
mdotst=thrust/deltaV
T=0
dt=0.0001
h=0
v=0
a=0
height=[]
time=[]
at=mdotst*(pressure*mat.sqrt(gamma/(intemp*specific_R))*mat.pow((gamma+1)/2,-(gamma+1)/(2*(gamma-1))))**-1  
while   1>0:
    h+=(v*dt)+(0.5*a*(dt**2))
    T+=0.0001
    height.append(h)
    time.append(T)
    if Mst>=drymass:
       Mst-=(mdotst*dt)
       thrust=twr*Mst*g
       mdotin=thrust/(60*g)
       a=(thrust-(Mst*g))/Mst
       v+=a*dt 
       mdotst=mdotin
       #if thrust>0:
       # print(thrust,mdotst)
    else:
        v-=(g*dt)
        if v<0.: break      
print(f"height={h}m ass={Mst}, thrust={thrust}  ,md={mdotin},time={T},Areat={at}") 
diametre= mat.sqrt( (4 / mat.pi) * (mdot * mat.sqrt(T)) / (pa * mat.sqrt(1.4 / specific_R) * 0.5787) )  
print("Diametre:",diametre)
at = math.pi * ((diametre/2)**2)
print("Area:",at) 

plt.figure(figsize=(10, 18))
plt.plot(time, height, label="Height (m)", marker="o")
plt.xlabel("Time (s)")
plt.ylabel("height")
plt.title("Rocket Performance Over Time")
plt.legend()
plt.grid(True)

# Show plot
plt.show()
#task 4
#conmdot'''
#dt=float(input("enter dt"))
#valueheight=max_height(netmass,massfintial,invol,twr,dt)
#print (valueheight)
#task4
'''data=[]
twr=float(input("enter twr intial:"))
Mst=netmass
pressure_ex=1.01325*(10**5)
pressure_atm=1.01325*(10**5)
thrust_intial=twr*Mst*g
mdot_intial=thrust_intial/(isp*g)
Mach=2*((pressure_ex/pressure)**((1 - gamma)/gamma) - 1)/(gamma - 1) 
Ve=Mach*(gamma*R*intemp)**0.5
diametre= mat.sqrt( (4 / mat.pi) * (mdot_intial * mat.sqrt(intemp)) / (pressure * mat.sqrt(1.4 / specific_R) * 0.5787) )
at = mat.pi * ((diametre/2)**2)
print("area",at)
dt=float(input("enter dt:"))
a=(thrust_intial-(Mst*g))/Mst
v=a*dt
h=(v*dt)+(0.5*a*(dt**2))
heights=[0,h]
#a=0
#v=0
T=dt
#h=0
mdot=mdot_intial
thrust=thrust_intial
data.append({
                'Time (s)': T,
                'Height (m)': h,
                'Velocity (m/s)': v,
                'Acceleration (m/s^2)': a,
                'Thrust (N)': thrust,
                'Mass (kg)': Mst,
                'Fuel Remaining (kg)': massfintial,
                'Mass Flow Rate (kg/s)': mdot,
                'Specific impulse(s)': isp,
                'Thrust to weight ratio': twr
                })

#print(f"velocity{v},acc={a},mdot={mdot_intial},mach{Mach},h={h}before loop")
while True:
    if massfintial>0:# thrust>(Mst*g):
       #print(f"mst={Mst}")
       Mst-=(mdot*dt)
       #print(f"mst={Mst}")
       massfintial-=mdot*dt
       #print(f"massfinal={massfintial}")
       pressure=(massfintial*R*intemp)/(0.028*invol)
       #print(f"pressure={pressure}")
       #print(f"mdot{mdot}")
       mdot=at*pressure*mat.sqrt(gamma/(intemp*specific_R))*mat.pow((gamma+1)/2,-(gamma+1)/(2*(gamma-1)))
       #print(f"mdot{mdot}")
       #Mach=2*((pressure_ex/pressure)**((1 - gamma)/gamma) - 1)/(gamma - 1) 
       Mach=mat.pow(((2/(gamma-1))*(((pressure_ex/pressure)**((1-gamma)/gamma))-1)),1/2)
       #print("mach",Mach,"mdot",mdot)
       ex_temp=intemp/(1+((gamma-1)/2)*(Mach**2))
       pressure_ex=pressure*mat.pow((1+((gamma-1)/2)*(Mach**2)),(-gamma/(gamma-1)))
       Ve=Mach*((gamma*specific_R*ex_temp)**0.5)
       ae=at*((2/gamma+1)*(1+(mfac*(Mach**2))/Mach))**efac
       #thrust=(mdot*Ve)+((pressure_ex-pressure)*ae)
       thrust=(mdot*Ve)+((pressure_ex-pressure_atm)*ae)
       #print(f"thrust={thrust},Mst={Mst*g},pressure difference={pressure_ex-pressure_atm}")
       a=(thrust-(Mst*g))/Mst
       v+=a*dt
       h=h+(v*dt)
       T+=dt
       isp=Ve/g
       twr=thrust/(Mst*g)
       heights.append(h)
       if  h<0 : 
            break 
       data.append({
                'Time (s)': T,
                'Height (m)': h,
                'Velocity (m/s)': v,
                'Acceleration (m/s^2)': a,
                'Thrust (N)': thrust,
                'Mass (kg)': Mst,
                'Fuel Remaining (kg)': massfintial,
                'Mass Flow Rate (kg/s)': mdot,
                'Specific impulse(s)': isp,
                'Thrust to weight ratio': twr
                })
       if  h<0 : 
            break  
    
    else:
        v-=(g*dt)
        h+=v*dt
        thrust=0
        mdot=0
        T+=dt
        if  h<0 : 
            break      
    #print(f"height={h} massf={massfintial}, thrust={thrust},mdot={mdot*dt}  ,acc={a},velocity={v}") 
print(f" maximum height of flight is:{max(heights)}")
    #plotting
times = [entry['Time (s)'] for entry in data]
heights = [entry['Height (m)'] for entry in data]
velocities = [entry['Velocity (m/s)'] for entry in data]
thrusts = [entry['Thrust (N)'] for entry in data]
massinst=[entry['Mass (kg)'] for entry in data]
mdots = [entry['Mass Flow Rate (kg/s)'] for entry in data]
accelerations = [entry['Acceleration (m/s^2)'] for entry in data]
isps = [entry['Specific impulse(s)'] for entry in data]
twrs = [entry['Thrust to weight ratio'] for entry in data]
plt.figure(figsize=(12, 6))

plt.subplot(2, 4, 1)
plt.plot(times, heights, label='Height', color='b')
plt.xlabel('Time (s)')
plt.ylabel('Height (m)')

plt.subplot(2, 4, 2)
plt.plot(times, velocities, label='Velocity', color='r')
plt.xlabel('Time (s)')
plt.ylabel('Velocity (m/s)')

plt.subplot(2, 4, 3)
plt.plot(times, accelerations, label='Acceleration', color='purple')
plt.xlabel('Time (s)')
plt.ylabel('Acceleration (m/s^2)')

plt.subplot(2, 4, 4)
plt.plot(times, thrusts, label='Thrust', color='g')
plt.xlabel('Time (s)')
plt.ylabel('Thrust (N)')

plt.subplot(2, 4, 5)
plt.plot(times, mdots, label='Mass Flow Rate', color='orange')
plt.xlabel('Time (s)')
plt.ylabel('Mass Flow Rate (kg/s)')

plt.subplot(2, 4, 6)
plt.plot(times, isps, label='Specific impulse value', color='y')
plt.xlabel('Time (s)')
plt.ylabel('Specific Impulse (s)')

plt.subplot(2, 4, 7)
plt.plot(times, twrs, label='Thrust to weight ratio', color='black')
plt.xlabel('Time (s)')
plt.ylabel('Thrust to Weight Ratio')

plt.subplot(2, 4, 8)
plt.plot(times, massinst, label='Massinstant', color='black')
plt.xlabel('Time (s)')
plt.ylabel('Massinstant')

plt.tight_layout()
plt.show()'''


# ??????????????????????????????????????????????????????????????????????????????????????
# enter 1 for sphere variation,
# enter 2 for cylinder variation,
# 3 for capsule variation:

o=3
if o==1:
#task 5 extra mass plot
    extrafuel=0
    T=0
    pressure_intial=60*(10**5)
    pressure=pressure_intial
    h=[]
    ext=[]
    radius=[]
    massfuel=[]
    volinside=[]
    mass=[]
    while True:
        #kk=int(input("")) 
        if r<=10 :
            if extrafuel<=100:
                print("mass fuel",massfintial)
                massfuel.append(massfintial)
                invol=(massfintial*R*intemp)/(0.028*pressure_intial)
                r=(3*invol/(4*mat.pi))**(1/3)
                radius.append(r)
                ttank=(pressure_intial*r*fos)/(4*zig)
                outvol=(((ttank+r)**3)*4*mat.pi)/3
                netvol=outvol-invol
                volinside.append(invol)
                Masstank=density*netvol
                #task=int(input("1 for the plot with tank,2 for plotwithout"))
                netmass=massfintial+enginemass+payload+Masstank
                mass.append(netmass)
            # print("INVOL",invol)
                #print(f"mass of the tank={Masstank},netmass={netmass},fuel={massfintial},tic={ttank},invol={invol},outvol={outvol}")
                #print(netmass,massfintial,invol,twr,dt)
                valueheight=max_height(netmass,massfintial,invol,twr,dt)
                #print (valueheight)
                #print("EXTRAFUEL:",extrafuel)
                #print("tankmass",Masstank)
                #print("massfuel",massfintial)
                if extrafuel==0:
                    print(netmass,massfintial,ttank,invol,dt)
                valueheight=max_height(netmass,massfintial,invol,twr,dt)
                print("height",valueheight)
                h.append(valueheight)
                ext.append(extrafuel)
                extrafuel+=1
                massfintial+=1

            else:break
        else: break



elif o==2:
    extrafuel=0
    T=0
    pressure_intial=60*(10**5)
    pressure=pressure_intial
    h=[]
    ext=[]
    cylinder=[]
    massfuel=[]
    len=0
    valueheight=0
    mass=[]
    volinside=[]
    while True:
        #kk=int(input("")) 
        if len<=10 :
            if extrafuel<=100:
                if valueheight<1000:
                    ext.append(extrafuel)
                # print("EXTRAFUEL:",extrafuel)
                    #print("mass fuel",massfintial)
                    massfuel.append(massfintial)
                    invol=0.01082369732142858    
                    r=(3*invol/(4*mat.pi))**(1/3)
                    cylinderinvol=(extrafuel*R*intemp)/(0.028*pressure_intial)
                    len=cylinderinvol/(mat.pi*(r**2))
                    #print("length=",len)
                    cylinder.append(len)
                    ttank=(pressure_intial*r*fos)/(2*zig)
                    cylinderoutvol=(mat.pi)*((r+ttank)**2)*len
                    outvol=0.011180494249779543
                    netvol=outvol-invol
                    cylindernetvol=cylinderoutvol-cylinderinvol
                # print("cylindernetvol",cylindernetvol)
                    totvol=netvol+cylindernetvol
                    netinvol=invol+cylinderinvol
                    volinside.append(netinvol)
                    Masstank=density*(netvol+cylindernetvol)
                    #task=int(input("1 for the plot with tank,2 for plotwithout"))
                    netmass=massfintial+enginemass+payload+Masstank
                    mass.append(netmass)
                # print("INVOL",invol)
                    #print(f"mass of the tank={Masstank},netmass={netmass},fuel={massfintial},tic={ttank},invol={invol},outvol={outvol}")
                    #print("total vol=",(netvol+cylindernetvol))
                    valueheight=max_height(netmass,massfintial,netinvol,twr,dt)
                    h.append(valueheight)
                    print ("height obtained",valueheight)
                    #print("tankmass",Masstank)
                    #print("massfuel",massfintial)
                    extrafuel+=1
                    massfintial+=1
                else:break

            else:break
        else:break


elif o == 3:
    capsule=[]
    T = 0
    pressure_intial= 60*(10**5)
    pressure = pressure_intial
    h=[]
    ext = []
    cylinder=[]
    massfuel= []
    len = 0
    valueheight = 0
    mass = []
    volinside =[]
    r=0.01
    radius=[]
    extrafuel=0
    fraction_fuel=0

    r_list=[] 
    l_list=[]  
    h_list=[]  
    print("Here, 1/nth of the fuel added to the tank is accomodated into the spherical part of the capsule ,while the rest is accomodated to yje cylinderical part ")
    print("the n in 1/nth of the fuel is here called fraction fuel")
    while fraction_fuel < 10:
        fraction_fuel += 1  # Increment fuel fraction
        extrafuel = 0
        h = []
        r = 0.01
        len = 0
        radius = []
        cylinder = []
        massfuel = []
        massfintial = massfintial_2
        plot = []
        valueheight=0
        while r < 2 and len <= 10 and extrafuel <= 100 and valueheight < 1000:
            ext.append(extrafuel)
            massfuel.append(massfintial)
            invol = (massfintial * R * intemp) / (0.028 * pressure_intial * fraction_fuel)
            r = (3 * invol / (4 * mat.pi)) ** (1 / 3)
            radius.append(r)
            ttank = (pressure_intial * r * fos) / (4 * zig)
            cttank = (pressure_intial * r * fos) / (2 * zig)
            outvol = (((ttank + r) ** 3) * 4 * mat.pi) / 3
            netvol = outvol - invol
            cylinderinvol = ((massfintial * (1 - (1 / fraction_fuel))) * R * intemp) / (0.028 * pressure_intial)
            len = cylinderinvol / (mat.pi * (r ** 2))
            cylinder.append(len)
            cylinderoutvol = (mat.pi) * ((r + cttank) ** 2) * len
            cylindernetvol = cylinderoutvol - cylinderinvol
            totvol = netvol + cylindernetvol
            netinvol = invol + cylinderinvol
            volinside.append(netinvol)
            Masstank = density * (netvol + cylindernetvol)
            netmass = massfintial + enginemass + payload + Masstank
            mass.append(netmass)
            
            #if extrafuel==0:
            #        print(netmass,massfintial,invol,twr,dt)
            valueheight = max_height(netmass, massfintial, netinvol, twr, dt)
            plot.append((massfintial, valueheight))
            h.append(valueheight)

            # Store values for 3D plotting
            r_list.append(r)
            l_list.append(len)
            h_list.append(valueheight)

            extrafuel += 1
            massfintial += 1
        print(len)
        plt.plot(massfuel, h, label=f"Fuel Fraction {fraction_fuel}")

   
    plt.title("Variation of Achieved Height with Fuel Mass")
    plt.xlabel("Fuel Mass")
    plt.ylabel("Achieved Height (m)")
    plt.legend() 
    plt.grid(True)  
    plt.show() 
    
    r_list = np.array(r_list)
    l_list = np.array(l_list)
    h_list = np.array(h_list)

    # Create 3D plot
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(r_list, l_list, h_list, c=h_list, cmap='viridis', marker='o')
    ax.set_xlabel("Radius (m)")
    ax.set_ylabel("Cylinder Length (m)")
    ax.set_zlabel("Achieved Height (m)")
    ax.set_title("3D Plot of Achieved Height vs Capsule Dimensions")

    plt.show()  # Show the 3D plot
         

        
if o==1:
    #print(massfuel)
    #print(radius)
    plt.plot(ext,h)
    plt.title('Height vs extramass')
    plt.xlabel('extramass')
    plt.ylabel('height')
    plt.show()

    plt.plot(radius,h) #task5
    plt.title('Height vs radius')
    plt.xlabel('radius')
    plt.ylabel('height')
    plt.show()


    plt.plot(ext,massfuel)
    plt.title('extrafuel vs massfuel')
    plt.xlabel('extrafuel')
    plt.ylabel('massfuel')
    plt.show()


    plt.plot(mass,massfuel)
    plt.title('netmass vs massfuel')
    plt.xlabel('netmass')
    plt.ylabel('massfuel')
    plt.show()


    plt.plot(volinside,h) #task5
    plt.title('Height vs total capacity ofsphere tank')
    plt.xlabel('capacity of tank')
    plt.ylabel('height')
    plt.show()
elif o==2:
    #print(massfuel)
    #print(ext)
    plt.plot(ext,h)
    plt.title('Height vs extramass')
    plt.xlabel('extramass')
    plt.ylabel('height')
    plt.show()

    plt.plot(cylinder,h)
    plt.title('Height vs length of cylinder ')
    plt.xlabel(' length of cylinder')
    plt.ylabel('height')
    plt.show()


    plt.plot(ext,massfuel)
    plt.title('extrafuel vs massfuel')
    plt.xlabel('extrafuel')
    plt.ylabel('massfuel')
    plt.show()

    plt.plot(mass,massfuel)
    plt.title('netmass vs massfuel')
    plt.xlabel('netmass')
    plt.ylabel('massfuel')
    plt.show()


    

    plt.plot(volinside,h) #task5
    plt.title('Height vs total capacity of combined  tank')
    plt.xlabel('capacity of tank')
    plt.ylabel('height')
    plt.show()

else:
    print("task has been completed")




'''
#task 5 height to get 1000
extrafuel=0
pressure_intial=60*(10**5)
pressure=pressure_intial
h=[]
ext=[]
massfintial=massfintial_2
while True:
    #kk=int(input("")) 
    if extrafuel<=100 :
        if valueheight<=1000:
            massfintial+=extrafuel
            invol=(massfintial*R*intemp)/(0.028*pressure_intial)
            #r=(3*invol/(4*mat.pi))**(1/3)
            #ttank=(pressure_intial*r*fos)/(2*zig)
            #outvol=(((ttank+r)**3)*4*mat.pi)/3
            #netvol=outvol-invol
            Masstank=4.4509
            #task=int(input("1 for the plot with tank,2 for plotwithout"))
            netmass=massfintial+enginemass+payload+Masstank
            print("INVOL",invol)
            print(f"mass of the tank={Masstank},netmass={netmass},fuel={massfintial},tic={ttank},invol={invol},outvol={outvol}")
            valueheight=max_height(netmass,massfintial,invol,twr,dt)
            print ("(height",valueheight)
            print("EXTRAFUEL:",extrafuel)
            print("tankmass",Masstank)
            print("massfuel",massfintial)
            valueheight=max_height(netmass,massfintial,invol,twr,dt)
            print("height",valueheight)
            h.append(valueheight)
            ext.append(extrafuel)
            extrafuel+=0.001
        else:break    
    else: break

print(h)
print(radius)
plt.plot(ext,h)
plt.title('Height vs extramass, when tank mass is not considerd')
plt.xlabel('extramass')
plt.ylabel('height')
plt.show()'''


#for l in range(1,30):
   # for
#k=valueheight(7.8,1.6,0.025,1.4,0.1)
#print(k)zz