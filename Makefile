#=============================================================================================================================================================================
VR_COMPILER = g++

VR_SRC = src
VR_BIN = bin
VR_OBJ = build

VR_PARCER_MAIN_NAME = parcer_main
VR_PARCER_NAME = parcer
VR_PARCER_BIN_NAME = vpn_parcer

VR_SRC_MAIN_NAME = tree_generator_main
VR_SRC_NAME = tree_generator
VR_BIN_NAME = mem_tree_by_pid

VR_FLAGS = -Wall -Wextra
#=============================================================================================================================================================================
main: tree_generator

all:tree_generator parcer
#=============================================================================================================================================================================
tree_generator:folders TREE_GENERATOR_OBJ PARCER_OBJ TREE_GENERATOR_MAIN_OBJ
	$(VR_COMPILER) $(VR_OBJ)/$(VR_SRC_MAIN_NAME).o $(VR_OBJ)/$(VR_SRC_NAME).o $(VR_OBJ)/$(VR_PARCER_NAME).o -o $(VR_BIN)/$(VR_BIN_NAME) $(VR_FLAGS)

TREE_GENERATOR_OBJ: $(VR_SRC)/$(VR_SRC_NAME).cpp
	$(VR_COMPILER) -c -o $(VR_OBJ)/$(VR_SRC_NAME).o $(VR_SRC)/$(VR_SRC_NAME).cpp  $(VR_FLAGS)

TREE_GENERATOR_MAIN_OBJ: $(VR_SRC)/$(VR_SRC_MAIN_NAME).cpp
	$(VR_COMPILER) -c -o $(VR_OBJ)/$(VR_SRC_MAIN_NAME).o $(VR_SRC)/$(VR_SRC_MAIN_NAME).cpp  $(VR_FLAGS)
#=============================================================================================================================================================================
parcer:folders PARCER_OBJ PARCER_MAIN_OBJ 
	$(VR_COMPILER) $(VR_OBJ)/$(VR_PARCER_NAME).o $(VR_OBJ)/$(VR_PARCER_MAIN_NAME).o -o $(VR_BIN)/$(VR_PARCER_BIN_NAME) $(VR_FLAGS)

PARCER_OBJ: $(VR_SRC)/$(VR_PARCER_NAME).cpp
	$(VR_COMPILER) -c -o $(VR_OBJ)/$(VR_PARCER_NAME).o $(VR_SRC)/$(VR_PARCER_NAME).cpp  $(VR_FLAGS)

PARCER_MAIN_OBJ: $(VR_SRC)/$(VR_PARCER_MAIN_NAME).cpp
	$(VR_COMPILER) -c -o $(VR_OBJ)/$(VR_PARCER_MAIN_NAME).o $(VR_SRC)/$(VR_PARCER_MAIN_NAME).cpp  $(VR_FLAGS)
#=============================================================================================================================================================================
folders:
	mkdir -p $(VR_OBJ);
	mkdir -p $(VR_BIN);
#=============================================================================================================================================================================
clean:
	rm -rf $(VR_OBJ)
	rm -rf $(VR_BIN)
#=============================================================================================================================================================================