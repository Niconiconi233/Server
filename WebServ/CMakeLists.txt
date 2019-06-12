cmake_minimum_required(VERSION 2.8)

project(test)

SET(SRC_LIST 
	Acceptor.cc
	Channel.cc
	Epoller.cc
	EventLoop.cc
	InetAddr.cc
	Socket.cc
	Buffer.cc
	Utils.cc
	TcpConnection.cc
	TcpServer.cc
	EventLoopThread.cc
	EventLoopThreadPool.cc
)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
ADD_LIBRARY (net STATIC ${SRC_LIST})
add_definitions("-Wall -g")
target_link_libraries(net pthread)
target_link_libraries(net log)
