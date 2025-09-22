CXX = g++
# -std=c++17 : Menggunakan standar C++ modern
# -Wall      : Menampilkan semua peringatan (praktik yang baik)
# -Irepo     : Memberi tahu compiler untuk mencari file header (.h) di folder 'repo'
CXXFLAGS = -std=c++17 -Wall -Irepo

# Nama program akhir yang akan dibuat
EXECUTABLE = main.exe

# Direktori tempat file source (.cpp) berada
SRC_DIR = service

# Direktori untuk menyimpan file objek (.o) hasil kompilasi sementara
OBJ_DIR = obj

# Secara otomatis menemukan semua file .cpp di DUA lokasi (root dan SRC_DIR)
SOURCES = $(wildcard *.cpp $(SRC_DIR)/*.cpp)

# Membuat daftar file objek (.o) dari nama file sumber, tanpa path direktorinya
OBJECTS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(SOURCES)))

# Memberi tahu 'make' untuk juga mencari source file di dalam SRC_DIR
VPATH = $(SRC_DIR)


# 3. Aturan (Rules)
# ----------------------------------------------------------
# Aturan default (dijalankan jika hanya mengetik 'make')
# Target 'all' bergantung pada program akhir kita
all: $(EXECUTABLE)

# Aturan untuk membuat program akhir (linking)
# Bergantung pada semua file objek
$(EXECUTABLE): $(OBJECTS)
	@echo "🔗 Linking object files into $(EXECUTABLE)..."
	$(CXX) $(CXXFLAGS) -o $@ $^

# Aturan Pola (Pattern Rule) untuk mengompilasi .cpp menjadi .o
# Ini adalah aturan cerdas yang berlaku untuk semua file .cpp
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	@echo "🔨 Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 4. Aturan Tambahan (Phony Targets)
# ----------------------------------------------------------
# Aturan untuk menjalankan program
run: all
	@echo "▶️  Running program..."
	./$(EXECUTABLE)

# Aturan untuk membersihkan file hasil build
clean:
	@echo "🧹 Cleaning up build files..."
	rm -rf $(OBJ_DIR) $(EXECUTABLE)

# Memberi tahu 'make' bahwa 'all', 'run', dan 'clean' bukan nama file
.PHONY: all run clean