# ==================================
# НАСТРОЙКИ
# ==================================
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pedantic
CXXFLAGS += -I.

# Папка для всех сгенерированных файлов
BUILD_DIR = build

# Имя исполняемого файла
TARGET = $(BUILD_DIR)/dos_tool

# Список исходных файлов (.cpp) с полными путями
SOURCES = \
	main.cpp \
	cli/cli.cpp \
	core/attacker.cpp \
	core/packet.cpp \
	utils/checksum.cpp

# Список чистых имен файлов (main.cpp, cli.cpp, ...)
SOURCE_NAMES = $(notdir $(SOURCES))

# Список объектных файлов, которые будут в папке build/
OBJS = $(addprefix $(BUILD_DIR)/, $(SOURCE_NAMES:.cpp=.o))

# Указываем Make, где искать исходные файлы (.cpp), которые указаны в OBJS
# В данном случае, это корневая папка, cli/, core/, utils/
VPATH = $(shell find . -type d)


# ==================================
# ОСНОВНЫЕ ПРАВИЛА
# ==================================

# Цель 'all' (по умолчанию): собирает TARGET
.PHONY: all
all: $(BUILD_DIR) $(TARGET)

# Цель: Создать основную папку сборки
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# 1. Основная цель: компоновка исполняемого файла
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# 2. Общее правило для компиляции:
# Использует VPATH для поиска исходных файлов в разных папках.
$(BUILD_DIR)/%.o: %.cpp
	@echo "Compiling $< to $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@


# ==================================
# ВСПОМОГАТЕЛЬНЫЕ ЦЕЛИ
# ==================================

# Цель 'clean': удаляет скомпилированные файлы и папку build
.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)

# Цель для перекомпиляции всего
.PHONY: rebuild
rebuild: clean all


# ... (Предыдущие секции)

# ==================================
# УСТАНОВКА И УДАЛЕНИЕ
# ==================================

# Папка для установки исполняемого файла
INSTALL_DIR = /usr/local/bin

# Цель 'install': копирует исполняемый файл и удаляет лишний шаг создания ссылки
.PHONY: install
install: all
	@echo "Installing $(notdir $(TARGET))..."
	# 1. Сначала убедимся, что папка установки существует
	@sudo mkdir -p $(INSTALL_DIR)
	
	# 2. Удаляем старую символическую ссылку или файл, если он существует
	@echo "Removing old installation (if any)..."
	@sudo rm -f $(INSTALL_DIR)/dos_tool
	
	# 3. Копируем исполняемый файл из папки сборки
	@echo "Copying executable to $(INSTALL_DIR)..."
	@sudo cp $(TARGET) $(INSTALL_DIR)
	
	# ШАГ 4 (СОЗДАНИЕ ССЫЛКИ) БОЛЬШЕ НЕ НУЖЕН
	
	@echo "Installation complete. You can now run 'dos_tool' from anywhere."

# Цель 'uninstall': удаляет исполняемый файл
.PHONY: uninstall
uninstall:
	@echo "Uninstalling $(notdir $(TARGET)) from $(INSTALL_DIR)..."
	# Удаляем сам исполняемый файл
	@sudo rm -f $(INSTALL_DIR)/dos_tool
	@echo "Uninstallation complete."
