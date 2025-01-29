int *getIc()
{
    static int *ic_pointer;
    static int ic = 0;
    ic_pointer = &ic;
    // if(!codeImage[0]){
    // }
    return ic_pointer;
}

void updateIc(int c)
{
    printf(" start updateIc c= %d\n", c);

    int *tmp = getIc();
    *tmp = *tmp + c;
    // printf(" *tmp %d \n", *tmp);
    // printf("  tmp %p \n", tmp);
}
void resetIc()
{

    int *tmp = getIc();
    *tmp = 0;
}