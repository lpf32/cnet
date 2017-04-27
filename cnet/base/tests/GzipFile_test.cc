#include <cnet/base/GzipFile.h>
#include <cnet/base/Logging.h>

#include <stdio.h>

int main()
{
    const char *filename = "/tmp/gzipfile_test.gz";
    ::unlink(filename);
    const char data[] = "12345679012jfadskljl324jljafldsj";
    {
        cnet::GzipFile writer = cnet::GzipFile::openForAppend(filename);
        if (writer.valid())
        {
            LOG_INFO << "tell " << writer.tell();
            LOG_INFO << "wrote " << writer.write(data);
            LOG_INFO << "tell " << writer.tell();
        }
    }

    {
        printf("test reader\n");
        cnet::GzipFile reader = cnet::GzipFile::openForRead(filename);
        if (reader.valid())
        {
            char buf[256];
            LOG_INFO << "tell " << reader.tell();
            int nr = reader.read(buf, sizeof buf);
            printf("read %d\n", nr);
            if (nr >= 0)
            {
                buf[nr] = '\0';
                printf("data %s", buf);
            }
            LOG_INFO << "tell " << reader.tell();
            if (strncmp(buf, data, strlen(data)) != 0)
            {
                printf("failed!!!\n");
                abort();
            }
            else
            {
                printf("PASSED!!!\n");
            }
        }
    }

    {
        cnet::GzipFile writer = cnet::GzipFile::openForWriteExclusive(filename);
        if (writer.valid() || errno != EEXIST)
        {
            printf("FAILED\n");
        }
    }
}
