# C source files
PROJECT_CSRC 	= main.c \
				  src/serial.c\
				  src/tcp/funcTCP.c\
				  src/tcp/threadsTCP.c\
				  tests/server_test.c\
				  tests/client_test.c\
				  src/modbus/modbusFunc.c \
				  src/modbus/modbusGet.c \
				  src/modbus/modbusRegister.c\
				  

# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= include\
				include/modbus\
				include/tcp\
				
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     = --specs=nosys.specs
# Additional .mk files are included here
