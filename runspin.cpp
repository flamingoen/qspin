#include "runspin.h"
using namespace std;

runSPIN::runSPIN()
{

}

string runSPIN::exec(string command) {
    cout << "using exec to run spin:" << endl;
    FILE *in;
    char buff[512];
    char *cmd = new char[command.length()+1];
    strcpy(cmd , command.c_str());
    cout << cmd << endl;
    string out;
    if(!(in = popen(cmd, "r"))){
        return "error";
    }
    while(fgets(buff, sizeof(buff), in)!=NULL){
        out += buff;
    }
    pclose(in);
    return out;
}
