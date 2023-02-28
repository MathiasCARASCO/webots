import math
from controller import Robot

# create the Robot instance
robot = Robot()

# get the time step of the simulation
timeStep = int(robot.getBasicTimeStep())

# get the proximity sensors
proximitySensors = []
lightSensors = []

proximitySensors.append(robot.getDevice("ds0"))
proximitySensors.append(robot.getDevice("ds1"))
proximitySensors.append(robot.getDevice("ds2"))
lightSensors.append(robot.getLightSensor("ls2"))
proximitySensors.append(robot.getDevice("ds3"))
proximitySensors.append(robot.getDevice("ds4"))
proximitySensors.append(robot.getDevice("ds5"))

# enable the proximity sensors
for sensor in proximitySensors:
    sensor.enable(timeStep)
    
# enable the light sensors
for sensor in lightSensors:
    sensor.enable(timeStep)
    

# get the motors
leftMotor = robot.getDevice("left wheel motor")
rightMotor = robot.getDevice("right wheel motor")

# set the initial speed of the motors
leftMotor.setPosition(float('inf'))
leftMotor.setVelocity(10.0)
rightMotor.setPosition(float('inf'))
rightMotor.setVelocity(10.0)

# run the robot simulation until an object is detected
while robot.step(timeStep) != -1:
    objectDetected = False
    # check if any of the proximity sensors has detected an object
    for sensor in proximitySensors:
        #print(sensor.getName(), sensor.getValue())
        print(lightSensors[0].getName(), lightSensors[0].getValue())
        if 0 < sensor.getValue() < 900:
            print("Object detected at", sensor.getValue())
            objectDetected = True
            break
    if objectDetected:
        # stop the motors
        leftMotor.setVelocity(0)
        rightMotor.setVelocity(0)
        break
print("finished")