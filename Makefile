CXX = g++

DEBUG = 1
ifeq ($(DEBUG), 1)
	CXXFLAGS = -std=c++11 -g
else
	CXXFLAGS += -O2
endif

source = main.cpp


server: $(source)
	$(CXX) -o server $^ $(CXXFLAGS) -lpthread -lmysqlclient

clean:
	rm -r server