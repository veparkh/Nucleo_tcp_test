# C source files
PROJECT_CSRC 	= main.c \
				src/serial.c\
				modbus/src/client/Requests.c\
				modbus/src/server/responseHandle.c\
				modbus/src/dataStructures.c\
				modbus/src/modbusFunc.c\
				modbus/src/requestHandler.c\
				tcp/src/client/TCPclientFunctions.c\
				tcp/src/server/TCPserverFunctions.c\
				tcp/src/server/Threads.c\
				tcp/src/funcTCP.c\
				tests/server_test.c\
				tests/client_test.c\
				
				
# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= include\
				modbus/include\
				tcp/include\
				
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     = --specs=nosys.specs
# Additional .mk files are included here
