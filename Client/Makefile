CXX = g++
CXXFLAGS = -Wall -pthread
OBJS = main.o wirelesscontroller/SafetyButton.o \
	   wirelesscontroller/Joypad.o \
	   utilities/Utilities.o

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o main $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) main