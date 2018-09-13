#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genann.h"
#include <string.h>

const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}


char** strsplit(char* str, const char* delim){
    char** res = NULL;
    char*  part;
    int i = 0;

    char* aux = strdup(str);

    part = strdup(strtok(aux, delim));

    while(part){
        res = (char**)realloc(res, (i + 1) * sizeof(char*));
        *(res + i) = strdup(part);

        part = strdup(strtok(NULL, delim));
        i++;
    }

    res = (char**)realloc(res, i * sizeof(char*));
    *(res + i) = NULL;

    return res;
}

int main()
{
    //from CSV to Double[]

    double inputs[8760][3];
    char line[128];
    int in=0;
    FILE* stream = fopen("in3.csv", "r");
    while (fgets(line, 128, stream))
    {
        char* tmp = strdup(line);

        char *ptr=NULL;
        char **res = strsplit(tmp,",");


        inputs[in][0]= strtod(res[0],&ptr)/152.03;//60.0;
        inputs[in][1]= strtod(res[1],&ptr)/29.4;//227.0;
        inputs[in][2]= strtod(res[2],&ptr)/100;//152.03;

        in++;

    }


    // from CSV to Output
    double outputs[8760];
    FILE* streamo = fopen("out3.csv", "r");
    int indexo=0;
    char line2[128];

    while (fgets(line2, 128, streamo))
    {
        char* tmp = strdup(line2);
        char *ptr=NULL;

        outputs[indexo]= strtod(tmp,&ptr)/339.0;;//350

        indexo++;
    }


    genann *ann = genann_init(3, 1, 7, 1);


    int count = 0;
    double err = 0;



    do
    {

        err=0;
        for(count=0; count<8760; count++)
            genann_train(ann,inputs[count],outputs+count,0.1);




        for(count=0; count<8760; count++)
            err+=pow(genann_run(ann, inputs[count])[0]-outputs[count],2.0);

        printf("error is %f\n",err);


    }
    while(err>38.0);





    FILE* out = fopen("ann.o", "w");

    genann_write(ann, out);

    double const *prediction = genann_run(ann, inputs[158]);
    printf("%f %f %f => %f & %f\nExpected:%f",inputs[158][0]*152.03,inputs[158][1]*29.4,inputs[158][2]*100.0,prediction[0]*339.0, prediction[1],outputs[158]*339);



    genann_free(ann);



    return 0;
}
