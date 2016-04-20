#include "testrun.h"

testRun::testRun(QString _path, int _type) : SpinRun(_path,_type) {}

testRun::start() {
    emit finished();
}
