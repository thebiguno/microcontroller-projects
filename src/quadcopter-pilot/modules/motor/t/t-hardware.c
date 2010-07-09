#define MOTOR_FRONT = MOTOR_A
#define MOTOR_RIGHT = MOTOR_B
#define MOTOR_REAR = MOTOR_C
#define MOTOR_LEFT = MOTOR_D

uint16_t[] motor_cmd(throttleCommandaxisCommand) {
    motorCommand[FRONT] = limitRange(transmitterCommand[THROTTLE] + motorAxisCommand[PITCH] - motorAxisCommand[YAW], minCommand, MAXCOMMAND);
    motorCommand[REAR] = limitRange(transmitterCommand[THROTTLE] - motorAxisCommand[PITCH] - motorAxisCommand[YAW], minCommand, MAXCOMMAND);
    motorCommand[RIGHT] = limitRange(transmitterCommand[THROTTLE] + motorAxisCommand[ROLL] + motorAxisCommand[YAW], minCommand, MAXCOMMAND);
    motorCommand[LEFT] = limitRange(transmitterCommand[THROTTLE] - motorAxisCommand[ROLL] + motorAxisCommand[YAW], minCommand, MAXCOMMAND);    
}