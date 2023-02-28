#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <webots/camera.h>
#include <webots/distance_sensor.h>
#include <webots/light_sensor.h>
#include <webots/motor.h>
#include <webots/robot.h>

#define MAX_SENSOR_NUMBER 16
#define LIGHT_THRESHOLD 25
#define RANGE (1024 / 2)
#define BOUND(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

static WbDeviceTag sensors[MAX_SENSOR_NUMBER], camera, left_motor, right_motor;
static double matrix[MAX_SENSOR_NUMBER][2];
static int num_sensors_dist;
static int num_sensors_light;
static double range;
static int time_step = 0;
static double max_speed = 0.0;
static double speed_unit = 4.0;

//static double k = 2;

/* We use this variable to enable a camera device if the robot has one. */
static int camera_enabled;

static void initialize() {
  /* necessary to initialize Webots */
  wb_robot_init();

  time_step = wb_robot_get_basic_time_step();

  const char *robot_name = wb_robot_get_name();
  
  const char khepera_name_dist[] = "ds0";
  const char khepera_name_light[] = "ls0";

  char sensors_name_dist[5];
  char sensors_name_light[5];
  const double(*temp_matrix)[2];

  camera_enabled = 0;
  range = RANGE;
  const double khepera_matrix[16][2] = { //{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                                       {-2, 4}, {-3, 5}, {-7, 7}, {7, -6}, {5, -4}, {4, -2}, {-0.5, -0.5}, {-0.5, -0.5},
                                       // {-4, 5}, {-5, 7}, {-8, 1}, {10, 9}, {-1, 8}, {7, -5}, {5, -4}, {-0.5, -0.5}};
                                        //{-3, 5}, {-4, 5}, {-7, 7}, {7, -6}, {5, -5}, {5, -3}, {-0.5, -0.5}, {-0.5, -0.5}
                                        {-2, 4}, {-3, 5}, {-7, 7}, {7, -6}, {5, -4}, {4, -2}, {-3, -2}, {-2, -3}
};

  if (strncmp(robot_name, "khepera", 7) == 0) {
    const double khepera_max_speed = 10.0;
    const double khepera_speed_unit = 3.0;

    num_sensors_dist = 8;
    num_sensors_light = 8;
    sprintf(sensors_name_dist, "%s", khepera_name_dist);
	sprintf(sensors_name_light, "%s", khepera_name_light);
    temp_matrix = khepera_matrix;
    max_speed = khepera_max_speed;
    speed_unit = khepera_speed_unit;
  } else {
    fprintf(stderr, "This controller doesn't support this robot\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < num_sensors_dist; i++) {
    sensors[i] = wb_robot_get_device(sensors_name_dist);
    wb_distance_sensor_enable(sensors[i], time_step);

    if ((i + 1) >= 10) {
      sensors_name_dist[2] = '1';
      sensors_name_dist[3]++;

      if ((i + 1) == 10) {
        sensors_name_dist[3] = '0';
        sensors_name_dist[4] = '\0';
      }
    } else
      sensors_name_dist[2]++;

    for (int j = 0; j < 2; j++)
      matrix[i][j] = temp_matrix[i][j];
  }
  
  for (int i = 0, ii = num_sensors_dist; i < num_sensors_light; i++, ii++) {
    sensors[ii] = wb_robot_get_device(sensors_name_light);
    wb_light_sensor_enable(sensors[ii], time_step);

    if ((i + 1) >= 10) {
      sensors_name_light[2] = '1';
      sensors_name_light[3]++;

      if ((i + 1) == 10) {
        sensors_name_light[3] = '0';
        sensors_name_light[4] = '\0';
      }
    } else
      sensors_name_light[2]++;

    for (int j = 0; j < 2; j++)
      matrix[ii][j] = temp_matrix[ii][j];
  }

  left_motor = wb_robot_get_device("left wheel motor");
  right_motor = wb_robot_get_device("right wheel motor");
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);

  if (camera_enabled == 1) {
    camera = wb_robot_get_device("camera");
    wb_camera_enable(camera, time_step);
  }

  printf("The %s robot is initialized, it uses %d distance sensors\n", robot_name, num_sensors_dist);
}

int main() {
  initialize();
 
  while (wb_robot_step(time_step) != -1) {
    double speed[2];
    double sensors_value[MAX_SENSOR_NUMBER];
    //double left_light_avg = 0.0;
    //double right_light_avg = 0.0;
  
    /* If there is a camera, we need to update refresh it. */
    if (camera_enabled == 1)
      wb_camera_get_image(camera);
/*
    // Récupération de la valeur moyenne des capteurs de lumière
    for (int i = 0; i < num_sensors_light/2; i++) {
      left_light_avg += (float) wb_light_sensor_get_value(sensors[i+num_sensors_dist]);
    }
    // Récupération de la valeur moyenne des capteurs de lumière
    for (int i = 0; i < num_sensors_light/2; i++) {
      right_light_avg += (float) wb_light_sensor_get_value(sensors[i+num_sensors_dist+num_sensors_light/2]);
    }
    left_light_avg = 1024-2.0*left_light_avg/(float)num_sensors_light;
    right_light_avg = 1024-2.0*right_light_avg/(float)num_sensors_light;
*/   
    //printf("%f\n", left_light_avg-right_light_avg);
	
    for (int i = 0; i < num_sensors_dist; i++)
      sensors_value[i] = wb_distance_sensor_get_value(sensors[i]);
    for (int i = num_sensors_dist; i < num_sensors_dist+num_sensors_light; i++)
      sensors_value[i] = wb_light_sensor_get_value(sensors[i]);

    /*
     * The Braitenberg algorithm is simple, it computes the
     * speed of each wheel by summing the value of each sensor multiplied by
     * its corresponding weight. That is why each sensor must have a weight
     * for each wheel.
     */
    for (int i = 0; i < 2; i++) {
      speed[i] = 0.0;

      for (int j = 0; j < num_sensors_dist+num_sensors_light; j++) {
        /*
         * We need to recenter the value of the sensor to be able to get
         * negative values too. This will allow the wheels to go
         * backward too.
         */
        speed[i] += speed_unit * matrix[j][i] * (1.0 - (sensors_value[j] / range));
      }

      speed[i] = BOUND(speed[i], -max_speed, max_speed);
    }
/* 
    // Si la source lumineuse est à gauche, augmenter la vitesse du moteur gauche pour tourner à gauche
    if (left_light_avg - LIGHT_THRESHOLD > right_light_avg) {
     speed[1] *= (1-k*left_light_avg/right_light_avg);
    }
    // Sinon, augmenter la vitesse du moteur droit pour tourner à droite
    else if (right_light_avg - LIGHT_THRESHOLD > left_light_avg) {
     speed[0] *= (1-k*right_light_avg/left_light_avg);
    }
    
    if (speed[0] < -max_speed) speed[0] = -max_speed;
    if (speed[1] < -max_speed) speed[1] = -max_speed;
    if (speed[0] > max_speed) speed[0] = max_speed;
    if (speed[1] > max_speed) speed[1] = max_speed;
*/

    /* Set the motor speeds */
    wb_motor_set_velocity(left_motor, speed[0]);
    wb_motor_set_velocity(right_motor, speed[1]);
  }

  return 0;
}
