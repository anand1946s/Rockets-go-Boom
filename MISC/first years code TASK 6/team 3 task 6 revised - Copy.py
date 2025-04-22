import math as m
import matplotlib.pyplot as plt

class Rocket:
    def __init__(self, height, dmass, pmass, Isp=68.3046767602, pressure_bar=60, pressure_pa=6000000, g=9.81, factor_of_safety=2,
               Tensile=276e6, Ct=300):
        # Constants
        self.Ct = Ct
        self.Isp = Isp
        self.pressure_bar = pressure_bar
        self.pressure_pa = pressure_pa
        self.g = g
        self.height = height
        self.dmass = dmass  # Mass of the launcher
        self.pmass = pmass  # Mass of the payload
        self.Tensile = Tensile  # Tensile strength
        self.factor_of_safety = factor_of_safety
        self.payload_mass = 1.5  # Mass of payload in Kg

    def calculate_delta_v(self):
        # Calculate change in velocity
        return m.sqrt(2 * self.g * self.height)

    def calculate_fuel_mass(self):
        # Calculate fuel mass, mass ratio, and other parameters
        dv = self.calculate_delta_v()
        Isp_values = self.Isp
        r_mo_mf = m.exp(dv / (self.Isp * self.g))  # Mass ratio (m0/mf)
        fmass = (r_mo_mf - 1) * (self.pmass + self.dmass)  # Fuel mass

        return fmass, self.dmass

    def calculate_tank_parameters(self, fuel_mass):
        # Calculate volume of fuel for the spherical part
        v_sphere = (fuel_mass * 1000 / 28.014) * 8.314 * self.Ct / self.pressure_pa  # in m^3

        # Calculate inner radius of the spherical tank
        R_sphere = ((v_sphere * 3) / (4 * m.pi)) ** (1 / 3)

        # Calculate thickness of the spherical tank
        thick_sphere = (self.pressure_pa * R_sphere * self.factor_of_safety) / (2 * self.Tensile)

        # Calculate mass of the spherical tank
        tank_mass_sphere = ((4 / 3) * m.pi * (((R_sphere + thick_sphere) ** 3) - (R_sphere ** 3))) * 2.7 * 1000

        # For the cylindrical part, keep the radius the same as the sphere's
        R_cylinder = R_sphere

        return tank_mass_sphere, R_sphere

    def calculate_cylinder_parameters(self, extra_fuel_mass, R_cylinder):
        # Calculate volume of extra fuel for the cylindrical part
        v_cylinder = (extra_fuel_mass * 1000 / 28.014) * 8.314 * self.Ct / self.pressure_pa  # in m^3

        # Calculate length of the cylindrical part
        L_cylinder = v_cylinder / (m.pi * R_cylinder ** 2)

        # Calculate thickness of the cylindrical tank
        thick_cylinder = (self.pressure_pa * R_cylinder * self.factor_of_safety) / (self.Tensile)

        # Calculate mass of the cylindrical tank
        tank_mass_cylinder = m.pi * ((R_cylinder + thick_cylinder) ** 2 - (R_cylinder) ** 2) * L_cylinder * 2.7 * 1000

        return tank_mass_cylinder, L_cylinder

    def calculate_new_fuel_mass(self, fuel_mass, tank_mass):
        # Calculate new fuel mass
        new_fuel_mass = (tank_mass + self.payload_mass + self.dmass) * (
                    m.exp(self.calculate_delta_v() / (self.Isp * self.g)) - 1)
        return new_fuel_mass

    def iterate_fuel_mass(self):
        # Initial fuel mass
        fuel_mass, dry_mass = self.calculate_fuel_mass()
        while True:
            tank_mass_sphere, R_sphere = self.calculate_tank_parameters(fuel_mass)
            new_fuel_mass = self.calculate_new_fuel_mass(fuel_mass + tank_mass_sphere, tank_mass_sphere)
            # print(f"The total rocket mass including payload is {self.payload_mass + new_fuel_mass + tank_mass_sphere + self.dmass}")
            print("Difference in fuel mass: ", new_fuel_mass - fuel_mass)

            if abs(new_fuel_mass - fuel_mass) <= 0.001:
                # print(f"The total rocket mass including payload is {self.payload_mass + new_fuel_mass + tank_mass_sphere + self.dmass}")
                print("Final fuel mass is: ", round(new_fuel_mass, 4))

                break

            fuel_mass = new_fuel_mass

        return fuel_mass, tank_mass_sphere, dry_mass

# User inputs
height = 1000  # float(input("Enter the value of height (in m): "))
dmass = 0.250  # float(input("Enter the mass of the launcher (in kg): "))
pmass = 1.5  # float(input("Enter the mass of the payload (in kg): "))
Ct = 300

# Create Rocket instance
rocket = Rocket(height, dmass, pmass)

# Iterate to find final fuel mass
ini_fuel_mass, f_tank_mass_sphere, dry_mass = rocket.iterate_fuel_mass()

# Constants for simulation
M = 1  # float(input("enter the mach number at throat : ")) mach number is taken as M
Total_rocket_mass = pmass + f_tank_mass_sphere + ini_fuel_mass + dry_mass
Isp = 68.3046767602
factor_of_safety = 2
pressure_pa = 6000000
Tensile = 276e6
g = 9.81
max_lst_alti = [0]
delta_f = 1
i = 0.200
lst_delta_f_count = [0]

while delta_f <= 50:

    
    print("delta f count is ", delta_f)
    lst_delta_f_count.append(delta_f)
    ini_fuel_mass += i

    delta_f += 1
    while True:
        # Radius of the spherical part
        tank_mass_sphere, R_sphere = rocket.calculate_tank_parameters(ini_fuel_mass)

        # Calculate extra fuel mass for the cylindrical part
        extra_fuel_mass = ini_fuel_mass * 0.1  # Example: 10% extra fuel

        # Calculate parameters for the cylindrical part
        tank_mass_cylinder, L_cylinder = rocket.calculate_cylinder_parameters(extra_fuel_mass, R_sphere)

        # Total tank mass
        tank_mass = tank_mass_sphere + tank_mass_cylinder

        # Calculate new fuel mass including the cylindrical part
        ff_mass = (tank_mass + 1.5 + 0.250) * (m.exp(((140)) / (68.3046767602 * 9.81)) - 1)

        if (ff_mass - ini_fuel_mass) <= 0.001:
            print(
                f"The total rocket mass including payload is {0.250 + ini_fuel_mass + 1.5} total fuel required is {ini_fuel_mass}")

            break

        print("ini_fuel_mass_2", ini_fuel_mass, "final fm", ff_mass)
        ini_fuel_mass = ff_mass

    Total_rocket_mass = ff_mass + 1.5 + 0.250 + tank_mass
    Isp = 68.304767602  # initial_new_calculated using mass flow rate and exhaust velocity
    g = 9.81
    thrust_to_weight = 1.4  # initial_given
    t = 0.001  # Time step in seconds
    pressure_pa = 6000000

    # Initial calculations
    Thrust = thrust_to_weight * Total_rocket_mass * g
    mdot_1 = Thrust / (670.688790179164)  # this value calculated using eqn

    print("thrust is : ", Thrust, "m dot is :", mdot_1)
    # Initialize variables
    lst_exh_velo = [0]
    lst_Thrust = [0]
    # initial mass flow rate is obtained through calculation
    lst_ISP = [68.304767602]
    lst_acceleration = [0]
    lst_TWR = [0]
    time = [0]
    lst_roct_velo = [0]
    lst_roct_altitude = [0]
    lst_mass_flow_rate = [mdot_1]  # Old value 0.2171857778
    chamber_pressure = [6000000]

    throat_area = mdot_1 / (6000000 * (m.sqrt(1.404 / (300 * 297)))*(m.pow(1 + ((1.404 - 1) / 2),((-1 / 2)*((1.404 + 1) / (1.404 - 1))))))  # 1.164100636e-5    # Old value 1.57641660e-5
    print("area ", throat_area)

    final_fuel_mass = ini_fuel_mass
    current_fuel_mass = ini_fuel_mass
    current_rocket_mass = Total_rocket_mass

    # Simulation loop
    def exit_area(throat_area):
        Ae = ((((1.404 + 1) / 2)**(-((1.404 + 1) / (2 * (1.404 - 1))))) * (
                    ((1 + (((1.404 - 1) / 2)*M*M)) ** ((1.404 + 1) / (2*(1.404 - 1)))) / M)) * throat_area

        return Ae
    area = exit_area(throat_area)
    print(f"the exhaust area is :{area}")

    def exhaust_velo(chamber_pressure):
        chamber_temp = Ct
        p_atm = (2 / 3) * 1.01325 * 100000
        R = 297  # the specific gas constant for gaseous nitrogen
        if chamber_pressure[-1] < 67551:
            exe_velo = 0
        else:

            exe_velo = m.sqrt(((((2 * 1.404) / (1.404 - 1)) * (R * Ct / M) * (
            (1 - ((p_atm / (chamber_pressure[-1])) ** (0.404 / 1.404)))))))

        return exe_velo

    def speci_imp():
        new_spec_impulse = (lst_exh_velo[-1]) / 9.81
        return new_spec_impulse

    def exhaust_temp(chamber_pressure):
        exhaust_pressure = 67550  # Exhaust pressure = ambient pressure (ideal condition)
        chamber_temp = Ct
        gamma = 1.404

        exhaust_t = Ct / (1 + 0.202 * M * M)
        return exhaust_t

    def pressure_nozzle(exhaust_velocity, area_throat):
        # Constants
        gamma = 1.404  # Specific heat ratio (Cp/Cv) for the gas
        mass_flow_rate = 0.1236233333  # Mass flow rate in kg/s

        # Calculate the nozzle area ratio (Ae/At)
        area_ratio = ((gamma + 1) / 2) ** (-(gamma + 1) / (2 * (gamma - 1))) * (
                (((1 + M * M * (gamma - 1) / 2)) ** ((gamma + 1) / (2 * (gamma - 1)))) / M)
        # include  M term
        # Calculate the nozzle exit area
        area_nozzle = area_throat * area_ratio

        # Calculate the gas density at the nozzle exit
        gas_density_nozzle = mass_flow_rate / (area_nozzle * exhaust_velocity)

        # Calculate the exhaust pressure
        # pressure_exhaust = 6000000 - 0.5 * gas_density_nozzle * exhaust_velocity ** 2
        # pressure_exhaust = gas_density_nozzle * 8.314 * exhaust_temp(6000000) * 1000 / (28.01)
        pressure_exhaust = (6000000 * (1 + (0.202) * M * M) ** (-1.404 / 0.404))
        return pressure_exhaust

    def mass_flow_rate():
        m_f_r = area * chamber_pressure[-1] * m.sqrt((1.404 / (297 * Ct)) * ((2 / 2.404) ** (2.404 / (2 * 0.404))))
        if chamber_pressure[-1] >= 67550:
            return m_f_r
        else:
            return 0

    def Thrust():
        new_thrust = lst_exh_velo[-1] * lst_mass_flow_rate[-1]

        return new_thrust  # input the list of exhaust velocity

    def acceleration():
        new_acceleration = (lst_Thrust[-1] - (current_rocket_mass * g)) / (current_rocket_mass)
        return new_acceleration

    def TWR(cur_roc_mass):  # input the current rocket mass and Thrust[-1]
        g = 9.81
        TWR = lst_Thrust[-1] / (cur_roc_mass * g)

        return TWR

    constant = pressure_pa / ini_fuel_mass

    def cha_pressure(cur_fuel_mass):
        new_pressure = constant * cur_fuel_mass

        return new_pressure

    while True:
        current_fuel_mass = current_fuel_mass - lst_mass_flow_rate[-1] * t
        lst_exh_velo.append(exhaust_velo(chamber_pressure))
        lst_mass_flow_rate.append(mass_flow_rate())
        chamber_pressure.append(cha_pressure(current_fuel_mass))
        current_rocket_mass -= lst_mass_flow_rate[-1] * t

        lst_ISP.append(speci_imp())
        exhaust_temp(chamber_pressure)

        lst_Thrust.append(Thrust())
        new_acceleration = (lst_Thrust[-1] - (current_rocket_mass * g)) / (current_rocket_mass)
        lst_acceleration.append(new_acceleration)
        lst_TWR.append(TWR(current_rocket_mass))

        final_velocity = lst_roct_velo[-1] + (lst_acceleration[-1]) * t
        lst_roct_velo.append(final_velocity)

        delta_h = lst_roct_altitude[-1] + lst_roct_velo[-1] * t
        lst_roct_altitude.append(delta_h)
        time.append(time[-1] + t)

        # print(f"height is {lst_roct_altitude[-1]} and velocity is {lst_roct_velo[-1]}")

        max_lst_alti.append(max(lst_roct_altitude))
        if lst_roct_altitude[-1] <= 0:
            break

    if max_lst_alti[-1] >= 1000:
        print(max_lst_alti[-1])
        break

    print(max_lst_alti[-1])

# Plotting
plt.figure(figsize=(12, 10))

plt.subplot(7, 1, 1)
plt.plot(time[:-1], lst_roct_velo[:-1], label="Velocity (m/s)", color="ORANGE")
plt.xlabel("Time (s)")
plt.ylabel("Velocity (m/s)")
plt.legend()

plt.subplot(7, 1, 2)
plt.plot(time[:-1], lst_acceleration[:-1], label="Acceleration (m/s²)", color="BROWN")
plt.xlabel("Time (s)")
plt.ylabel("Acceleration (m/s²)")
plt.legend()

plt.subplot(7, 1, 3)
plt.plot(time[:-1], lst_Thrust[:-1], label="Thrust (N)", color="BLUE")
plt.xlabel("Time(s)")
plt.ylabel("Thrust (N)")
plt.legend()

plt.subplot(7, 1, 4)
plt.plot(time[:-1], lst_TWR[:-1], label="TWR ", color="PURPLE")
plt.xlabel("Time (s)")
plt.ylabel("TWR ")
plt.legend()

plt.subplot(7, 1, 5)
plt.plot(time[:-1], lst_roct_altitude[:-1], label="Altitude (meters)", color="BLACK")
plt.xlabel("Time (s)")
plt.ylabel("Altitude (meters)")
plt.legend()

plt.subplot(7, 1, 6)
plt.plot(time[:-1], lst_ISP[:-1], label="Specific impulse (s)", color="GREEN")
plt.xlabel("Time (s)")
plt.ylabel("Specific impulse (s)")
plt.legend()

plt.subplot(7, 1, 7)
plt.plot(time[:-1], lst_mass_flow_rate[:-1], label="Mass flow rate(Kg/s)", color="RED")
plt.xlabel("Time (s)")
plt.ylabel("Mass flow rate(Kg/s)")
plt.legend()

plt.tight_layout()
plt.show()
