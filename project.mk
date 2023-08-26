# C source files
PROJECT_CSRC 	= main.c \
				  src/serial.c \
				  tests/server_test.c\
				  tests/client_test.c\
				  modbusTCP/modbusFunc.c \
				  modbusTCP/modbusGet.c \
				  modbusTCP/modbusRegister.c\

# C++ source files
PROJECT_CPPSRC 	= 
# Directories to search headers in
PROJECT_INCDIR	= include\
				include/modbusTCP\
# Additional libraries
PROJECT_LIBS	=
# Compiler options
PROJECT_OPT     = --specs=nosys.specs
# Additional .mk files are included here
