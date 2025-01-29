static int instructionCount = 0;
// Getter function to retrieve the value
int getInstructionCount()
{
    return instructionCount;
}

// Setter function to set the value
void setInstructionCount(int value)
{
    instructionCount = value;
}

// Update function to modify the value by a specific delta
void updateInstructionCount(int delta)
{
    instructionCount += delta;
 }
