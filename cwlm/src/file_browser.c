
#define LOSSY 0
#define LOSSLESS 1

struct File {
    char name[16];
    uint8_t minutes;
    uint16_t seconds;
    uint8_t compression_type;
};

struct File File_One;
struct File File_Two;

struct File file_list[2] = {
    File_One,
    File_Two,
};

int main() {
    // strcpy((File_One.name), "file1");
    File_One.minutes = 2;
    File_One.seconds = 127;
    File_One.compression_type = LOSSY;

    struct File File_Two;
    // strcopy(File2.name, "file2");
    File_Two.minutes = 3;
    File_Two.seconds = 200;
    File_Two.compression_type = LOSSLESS;

    return 1;

}