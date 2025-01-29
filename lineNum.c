static int lineNum = 1;
// Getter function to retrieve the value
int getLineNum()
{
    return lineNum;
}

// Setter function to set the value
void setLineNum(int value)
{
    printf("setLineNum \n");

    lineNum = value;
}

// Update function to modify the value by a specific delta
void updateLineNum(int delta)
{
    lineNum += delta;
}
