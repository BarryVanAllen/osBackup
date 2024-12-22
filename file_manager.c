#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Reads the content of a file into a dynamically allocated buffer.
 * 
 * @param filename The name of the file to read.
 * @param buffer_size The maximum size of the buffer to allocate.
 * @return A pointer to the buffer containing file contents, or NULL on failure.
 *         Caller must free the buffer after use.
 */
char *read_file(const char *filename, size_t *buffer_size) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size < 0) {
        perror("Error getting file size");
        fclose(file);
        return NULL;
    }

    // Allocate buffer to hold file contents
    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';  // Null-terminate the buffer

    fclose(file);

    if (buffer_size) {
        *buffer_size = bytes_read;
    }

    return buffer;
}
/**
 * Writes data to a file.
 * 
 * @param filename The name of the file to write to.
 * @param data The data to write to the file.
 * @param append If non-zero, data will be appended; otherwise, the file will b>
 * @return 0 on success, or -1 on failure.
 */
int write_file(const char *filename, const char *data, int append) {
const char *mode = append ? "a" : "w";
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    size_t data_length = strlen(data);
    size_t bytes_written = fwrite(data, 1, data_length, file);

    fclose(file);

    if (bytes_written < data_length) {
        fprintf(stderr, "Error writing to file\n");
        return -1;
    }

    return 0;
}
/**
 * Copies the contents of one file to another.
 * 
 * @param source_file The source file to copy from.
 * @param destination_file The destination file to copy to.
 * @return 0 on success, or -1 on failure.
 */
int copy_file(const char *source_file, const char *destination_file) {
    size_t buffer_size;
    char *buffer = read_file(source_file, &buffer_size);
    if (buffer == NULL) {
        return -1;
    }

    if (write_file(destination_file, buffer, 0) != 0) {
        free(buffer);
        return -1;
    }
free(buffer);
    return 0;
}



int main() {
    const char *source = "source.txt";
    const char *destination = "destination.txt";

    // Écrire des données dans un fichier
    if (write_file(source, "Ceci est un test.\n", 0) == 0) {
        printf("Fichier écrit avec succès : %s\n", source);
    } else {
        fprintf(stderr, "Erreur lors de l'écriture du fichier.\n");
        return 1;
    }

    // Copier le fichier
    if (copy_file(source, destination) == 0) {
        printf("Fichier copié avec succès vers : %s\n", destination);
    } else {
        fprintf(stderr, "Erreur lors de la copie du fichier.\n");
        return 1;
    }

    return 0;
}




