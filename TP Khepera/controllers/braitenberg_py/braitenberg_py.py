import math

# import the necessary Webots classes
from controller import Robot, Motor, DistanceSensor, Camera

# define some constants
MAX_SENSOR_NUMBER = 16
RANGE = (1024 / 2)

# initialize some global variables
sensors = [None] * MAX_SENSOR_NUMBER
left_motor = None
right_motor = None
camera = None
matrix = [[0.0, 0.0] for _ in range(MAX_SENSOR_NUMBER)]
num_sensors = 0
range = RANGE
time_step = 0
max_speed = 0.0
speed_unit = 1.0
camera_enabled = False

def initialize():
    global time_step
    global num_sensors
    global range
    global max_speed
    global speed_unit
    global camera_enabled

    robot = Robot()
    time_step = int(robot.getBasicTimeStep())
    robot_name = robot.getName()

    e_puck_name = "ps0"
    khepera_name = "ds0"
    koala_name = "ds0"
    pioneer2_name = "ds0"
    temp_matrix = None

    if robot_name == e_puck_name:
        num_sensors = 8
        temp_matrix = [[150, -35], [100, -15], [80, -10], [-10, -10], [-10, -10], [-10, 80], [-30, 100], [-20, 150]]
    elif robot_name == khepera_name:
        num_sensors = 8
        temp_matrix = [[-2, 4], [-3, 5], [-7, 7], [7, -6], [5, -4], [4, -2], [-0.5, -0.5], [-0.5, -0.5]]
    elif robot_name == koala_name:
        num_sensors = 16
        temp_matrix = [[17, -1], [8, -2], [4, -
3], [9, -2], [5, -3], [-4, -2], [-4, -3], [5, -3],
                       [5, -3], [-5, 5], [-3, 4], [3, -4], [5, -5], [5, -2], [2, -1], [1, -1]]
    elif robot_name == pioneer2_name:
        num_sensors = 16
        temp_matrix = [[-1, 15], [-3, 13], [-3, 8], [-2, 7], [-3, -4], [-4, -2], [-3, -2], [-1, -1],
                       [-1, -1], [-2, -3], [-2, -4], [4, -3], [7, -5], [7, -3], [10, -2], [11, -1]]

    if temp_matrix is not None:
        for i in range(num_sensors):
            matrix[i][0] = temp_matrix[i][0]
            matrix[i][1] = temp_matrix[i][1]

    left_motor = robot.getDevice("left wheel motor")
    right_motor = robot.getDevice("right wheel motor")
    left_motor.setPosition(float('inf'))
    right_motor.setPosition(float('inf'))
    left_motor.setVelocity(0.0)
    right_motor.setVelocity(0.0)

    for i in range(num_sensors):
        sensors[i] = robot.getDevice("ds" + str(i))
        sensors[i].enable(time_step)

    camera = robot.getDevice("camera")
    if camera:
        camera.enable(time_step)
        camera_enabled = True

    max_speed = left_motor.getMaxVelocity()
    speed_unit = max_speed / RANGE
