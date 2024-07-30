#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compressor.h"
#include "decompressor.h"

void print_menu() {
    printf("Escolha uma opção:\n");
    printf("1. Comprimir arquivo\n");
    printf("2. Descomprimir arquivo\n");
    printf("3. Sair\n");
}

int main() {
    int choice;
    char filename[256], output[256];
    unsigned long long original_size, compressed_size;

    while (1) {
        print_menu();
        scanf("%d", &choice);

        if (choice == 3) {
            printf("Encerrando o programa.\n");
            break;
        }

        printf("Digite o nome do arquivo (com extensão): ");
        scanf("%s", filename);

        if (choice == 1) {
            printf("Digite o nome do arquivo de saída: ");
            scanf("%s", output);
            original_size = get_file_size(filename);
            compress_file(filename, output);
            compressed_size = get_file_size(output);
            printf("Compressão concluída.\n");
            printf("Tamanho original: %llu bytes\n", original_size);
            printf("Tamanho comprimido: %llu bytes\n", compressed_size);
            printf("Taxa de compressão: %.2f%%\n", 100.0 * (original_size - compressed_size) / original_size);
        } else if (choice == 2) {
            printf("Digite o nome do arquivo de saída: ");
            scanf("%s", output);
            decompress_file(filename, output);
            printf("Descompressão concluída.\n");
        } else {
            printf("Opção inválida, tente novamente.\n");
        }
    }

    return 0;
}
