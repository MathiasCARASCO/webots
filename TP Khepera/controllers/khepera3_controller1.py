from controller import Robot, Motor, DistanceSensor

# Create a new instance of the Robot class
robot = Robot()

# Get a reference to the left wheel of the robot
leftWheel = robot.getMotor("left wheel")

# Get a reference to the right wheel of the robot
rightWheel = robot.getMotor("right wheel")

# Set the target velocity of the wheels
leftWheel.setPosition(float('inf'))
rightWheel.setPosition(float('inf'))

# Get reference to the distance sensors
ds = [robot.getDistanceSensor(name) for name in ['ds_right', 'ds_left']]

# Enable the distance sensors
for sensor in ds:
    sensor.enable(32)

# Set a desired linear velocity
velocity = 0.5

while robot.step(32) != -1:
    # Get the values from the distance sensors
    sensor_values = [sensor.getValue() for sensor in ds]

    # Calculate the error based on the sensor values
    error = sensor_values[0] - sensor_values[1]

    # Calculate the new target velocities for the wheels
    left_velocity = velocity + error
    right_velocity = velocity - error

    # Set the new target velocities for the wheels
    leftWheel.setVelocity(left_velocity)
    rightWheel.setVelocity(right_velocity)