CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = main
SRCS     = main.cpp ProtocolServer.cpp \
           GopherServer.cpp GopherConnection.cpp \
           GeminiServer.cpp GeminiConnection.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) -lssl -lcrypto

clean:
	rm -f $(TARGET)
