int *getDc()
{
    static int *dc_pointer;
    static int dc = 0;
    dc_pointer = &dc;
    return dc_pointer;
}

void updateDc(int c)
{

    int *tmp = getDc();
    *tmp = *tmp + c;
    // printf(" *tmp %d \n", *tmp);
    // printf("  tmp %p \n", tmp);
}
void resetDc()
{

    int *tmp = getDc();
    *tmp = 0;
}