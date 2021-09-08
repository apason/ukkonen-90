#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/* Input data must be uniqued! */

// TARKISTA VAIKUTTAAKO SORTTAUKSEN AIKAAN SE ETTÄ ON JO SORTATTU!

void main(void){

    FILE *input = fopen("input", "rb");
    FILE *output;
    char output_name[25];

    char buffer[103];

    if(input == NULL){
        perror("fopen");
        return;
    }

    srand(time(NULL));

    size_t last_line = 3261975;
    char * index = malloc(sizeof(*index) * last_line);
    
    size_t wanted_size = 3200000;

    int current;

    for(int i = 11; i > 0; i--){
        
        sprintf(output_name, "%X_dna_%ld.acgt", i, wanted_size);
        output = fopen(output_name, "w");

        memset(index, 0, sizeof(*index) * last_line);

        for(int j = 0; j < wanted_size; j++){

            current = rand() % last_line;
            if(index[current] != 0){
                j--;
                continue;
            }

            index[current] = 1;

            fseek(input, current * (101), SEEK_SET);

            memset(buffer, 0, sizeof(buffer));
            if(fread(buffer, 1, 101, input) != 101){
                j--;
                index[current] = 0;
                continue;
            }

            fwrite(buffer, 101, 1, output);

        }

        fclose(output);

        wanted_size /= 2;

    }

    fclose(input);

}
