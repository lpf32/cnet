#include <cnet/base/LogFile.h>
#include <cnet/base/Logging.h>
#include <boost/scoped_ptr.hpp>
#include <cnet/base/FileUtil.h>
#include <stdio.h>

using namespace cnet;

boost::scoped_ptr<LogFile> g_logfile;

void outputFunc(const char* msg, int len)
{
    g_logfile->append(msg, len);
}

void flushFunc()
{
    g_logfile->flush();
}

int main(int argc, char *argv[])
{
    char name[256];
    strncpy(name, argv[0], 256);
    g_logfile.reset(new LogFile(::basename(name), 200*1000));
    Logger::setOutput(outputFunc);
    Logger::setFlush(flushFunc);

    string line = "1234567890 abcdefhigkljl12fkl ";

    for (int i = 0; i < 1000; ++i) {
        LOG_INFO << line << i;
        usleep(1000);
    }

}