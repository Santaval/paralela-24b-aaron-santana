include ../../common/Makefile

DEPS += p7zip-full # Instala el paquete p7zip-full

# Variables
FILES = job001b.7z job002b.7z job003b.7z job020-input.7z
URLS = https://jeisson.ecci.ucr.ac.cr/concurrente/2024b/tareas/heatsim/
DEST_DIR = tests/jobs

# Regla principal para la descarga, descompresión y ejecución
init_test: clean_test download run_tests

# Asegura que el directorio de destino exista antes de descargar
$(DEST_DIR):
	@mkdir -p $(DEST_DIR)

# Regla para descargar y descomprimir cada archivo en su propia carpeta
download: $(FILES)

# Reglas dinámicas para descargar y descomprimir cada archivo
$(FILES): | $(DEST_DIR)
	@echo "Descargando $@ en $(DEST_DIR)..."
	@wget -P $(DEST_DIR) $(URLS)$@
	@echo "Descomprimiendo $@..."
	@mkdir -p $(DEST_DIR)/$(basename $@)  # Crear subdirectorio para cada archivo
	@7z x -o$(DEST_DIR)/$(basename $@) $(DEST_DIR)/$@

# Regla para limpiar los archivos descargados y descomprimidos
clean_tests:
	@echo "Eliminando archivos en $(DEST_DIR)..."
	@rm -f $(DEST_DIR)/*.7z
	@rm -rf $(DEST_DIR)/*/

# Regla para ejecutar el programa en cada archivo .txt descomprimido y comparar los resultados
run_tests: 
	@echo "Ejecutando bin/serial sobre cada archivo .txt..."
	@find $(DEST_DIR) -type f -name '*.txt' | while read -r file; do \
		echo "Ejecutando bin/serial sobre $$file..."; \
		bin/pthreads $$file 8 $$ARGS; \
		icdiff $${file%.txt}.tsv $${file%.txt}.out || echo "Diferencias encontradas en $$file"; \
	done
	$(MAKE)   # Ejecuta la comparación después de cada ejecución

FLAGS += -pthread
