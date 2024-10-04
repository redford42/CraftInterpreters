#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"

int tokenArrayCount = 0;

Token *tokenStart;

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        printf("goes on files %s\n", argv[1]);
        // =======================读取文件
        char *path = argv[1];
        FILE *file = fopen(path, "rb");
        if (file == NULL)
        {
            fprintf(stderr, "Could not open file \"%s\".\n", path);
            exit(74);
        }
        fseek(file, 0L, SEEK_END);
        size_t fileSize = ftell(file);
        rewind(file);

        char *buffer = (char *)malloc(fileSize + 1);
        if (buffer == NULL)
        {
            fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
            exit(74);
        }
        //< no-buffer
        size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
        //> no-read
        if (bytesRead < fileSize)
        {
            fprintf(stderr, "Could not read file \"%s\".\n", path);
            exit(74);
        }

        //< no-read
        buffer[bytesRead] = '\0';
        fclose(file);
        // =======================读取文件
        Token tokenArray[bytesRead];
        initScanner(buffer);
        fprintf(stderr, "initScanner \"%s\".\n", buffer);
        scanAllToken(tokenArray);
        // =======================完成文件字节读取，开始分析
        for (size_t i = 0; i < bytesRead; i++)
        {
             fprintf(stderr, "Token temp \"%d\".\n", tokenArray[i].type);
            /* code */
        }
        

    }
    else if (argc > 2)
    {
        printf("Too many arguments supplied.\n");
        // maybe runPrompt
    }
    else
    {
        printf("One argument expected.\n");
    }
    system("pause");
}