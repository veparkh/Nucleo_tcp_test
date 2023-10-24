# C source files
PROJECT_CSRC 	= main.c \
				src/serial.c\
				modbus/src/client/Requests.c\
				modbus/src/server/responseHandle.c\
				modbus/src/dataStructures.c\
				tcp/src/client/TCPclientFunctions.c\
				tcp/src/server/TCPserverFunctions.c\
				tcp/src/server/TCPserverThreads.c\
				tcp/src/funcTCP.c\
				tests/server_test.c\
				tests/client_test.c\
				modbus/src/modbusFunc.c
				
# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= include\
				modbus/include\
				modbus/include\server\
				modbus/include\client\
				tcp/include\
				tcp/include\server\
				tcp/include\client\
				
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     = --specs=nosys.specs
# Additional .mk files are included here
