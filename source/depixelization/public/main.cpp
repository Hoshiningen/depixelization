
#include <ProgramDriver.h>

int main(int argc, char* argv[])
{
    ProgramDriver driver{ "depixelization", argc, argv };
    return driver.go();
}
