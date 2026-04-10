CXX = g++
CXXFLAGS = -Wall -pthread
OBJS = main.o environment/Environment.o \
	   robot/components/battery/Battery.o \
	   robot/components/gripper/Gripper.o \
	   robot/components/movingbase/MovingRobot.o \
	   robot/components/roboticarm/RoboticArm.o \
	   robot/Robot.o \
	   utilities/Utilities.o

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o main $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) main