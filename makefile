CC = g++
CFLAGS = -Wall -Wextra -std=c++11
LDFLAGS = -lGLU -lGL -lglut -lGLEW -lm -lSOIL

OBJDIR = obj

SOURCES = main.cpp
LIBS = libs/bibutil.cpp
EXECUTABLE = app
SOILDIR = libs/SOIL
SOILLIBDIR = libs/SOIL/lib


.PHONY: all clean clone_soil install_soil

all: clone_soil install_soil $(EXECUTABLE)
	@echo "Build completed. Running the application..."
	@./$(EXECUTABLE)
	@echo "Cleaning up..."
	@$(MAKE) clean

$(EXECUTABLE):
	$(CC) -o $(EXECUTABLE) $(LIBS) $(SOURCES) $(LDFLAGS)

clone_soil:
	@if [ ! -d "$(SOILDIR)" ]; then \
		git clone https://github.com/kbranigan/Simple-OpenGL-Image-Library.git $(SOILDIR); \
	else \
		echo "SOIL directory already exists."; \
	fi

install_soil:
	@echo "Instalando pacotes..."
	@if [ -f "$(SOILLIBDIR)/libSOIL.a" ]; then \
		echo "SOIL library already installed. Skipping installation."; \
	else \
		$(MAKE) -C $(SOILDIR); \
		sudo $(MAKE) -C $(SOILDIR) install; \
	fi

clean:
	rm -rf $(EXECUTABLE)

clean-libs:
	rm -rf $(SOILDIR)
