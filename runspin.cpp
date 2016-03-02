#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <mainwindow.h>
#include <unistd.h>
using namespace std;

#ifdef OS_WINDOWS
    #define SPIN "spin/spin.exe"
#else
    #define SPIN "spin"
#endif

string exec(string command) {
    FILE *in;
    char buff[512];
    char *cmd = new char[command.length()+1];
    strcpy(cmd , command.c_str());
    string out;
    if(!(in = popen(cmd, "r"))){
        return "error";
    }
    while(fgets(buff, sizeof(buff), in)!=NULL){
        cout << buff << endl;
        out += buff;
    }
    pclose(in);
    return buff;
}

string runRandomSim(string file, int steps){
    string out;
    stringstream ss;
    ss << SPIN << " -u" << steps << " -p -l -g " << file;
    cout << ss.str() << endl;
    return exec(ss.str());
}

string runVer(string file,string rOptions,string cOptions){
    cout << "Running verification:" << endl;
    string out;
    exec("rm pan*");
    exec(SPIN  " -a "+file);
    cout <<"cc "+cOptions+"-o pan pan.c"<< endl;
    usleep(100);
    exec("cc "+cOptions+"-o pan pan.c");
    usleep(100);
    cout << "./pan " + rOptions << endl;
    return exec("./pan " + rOptions);
}
