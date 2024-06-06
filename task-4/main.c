#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

void print_json(cJSON *json) 
{
    if (json->type == cJSON_Object) 
    {
        cJSON *child = json->child;
        while (child != NULL) 
        {
            printf("%s: ", child->string);
            print_json(child);
            child = child->next;
        }
    } 
    else if (json->type == cJSON_Array) 
    {
        cJSON *child = json->child;
        while (child != NULL) 
        {
            print_json(child);
            child = child->next;
        }
    } 
    else if (json->type == cJSON_String) 
    {
        printf("%s\n", json->valuestring);
    } 
    else if (json->type == cJSON_Number) 
    {
        printf("%g\n", json->valuedouble);
    } 
    else if (json->type == cJSON_True) 
    {
        printf("true\n");
    } 
    else if (json->type == cJSON_False) 
    {
        printf("false\n");
    } 
    else if (json->type == cJSON_NULL) 
    {
        printf("null\n");
    }
}

int main() 
{
    FILE *file = fopen("data.json", "r");
    if (file == NULL) 
    {
        printf("Error opening file\n");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *json_string = (char *)malloc(file_size + 1);
    fread(json_string, 1, file_size, file);
    fclose(file);
    json_string[file_size] = '\0';

    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) 
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) 
        {
            printf("Error before: %s\n", error_ptr);
        }
        return 1;
    }

    print_json(json);

    cJSON_Delete(json);
    free(json_string);

    return 0;
}
