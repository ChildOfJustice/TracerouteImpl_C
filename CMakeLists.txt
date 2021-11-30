cmake_minimum_required(VERSION 3.20)
project(TracerouteProject C)

set(CMAKE_C_STANDARD 99)

add_executable(TracerouteProject myOwnTraceroute.c workingTraceroute.c test1/icmp_listener.c test1/pktInjector.c icmp_listener.h)
