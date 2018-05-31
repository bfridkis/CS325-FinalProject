#####################################################
## Program name: Makefile
## Author: Benjamin Fridkis
## Date: 5/23/2018
## Description: Makefile for TSP Project (CS325-400)
#####################################################

CXX = g++
CXXFLAGS = -std=c++11
CXXFLAGS += -Wall
#CXXFLAGS += Werror
CXXFLAGS += -pedantic-errors
CXXFLAGS += -g
#CXXFLAGS+= -03
#LDFLAGS = -lboost_date_time

OBJS1 = greedyTSP_w2Opt.o

SRCS1 = greedyTSP_w2Opt.cpp

HEADERS = vector2.h triangle.h delaunay.h

PROGRAM1_NAME = greedyTSP_w2Opt

${PROGRAM1_NAME}: ${OBJS1} ${HEADERS}
	${CXX} ${LDFLAGS} ${OBJS1} -o ${PROGRAM1_NAME}
	
${OBJS1}: ${SRCS1}
	${CXX} ${CXXFLAGS} -c $(@:.o=.cpp)	
	
run:
	./${PROGRAM1_NAME}
	
clean:
	rm *.o ${PROGRAM1_NAME}
