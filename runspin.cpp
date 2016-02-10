#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <mainwindow.h>
using namespace std;

#ifdef OS_WINDOWS
    #define SPIN "spin/spin.exe"
#else
    #define SPIN "spin"
#endif

string exec(string command) {
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

string runSim(string file){
    string out;
    return exec(SPIN " -u200 -p "+file);
}

string runVer(string file){
    string out;
    exec(SPIN  "-a "+file);
    exec("cc -o pan pan.c");
    return exec("./pan " + file);
}
